#include "inference/engine.h"
#include "utils/config.h"
#include "utils/logger.h"
#include "translation/data_structures.h"
#include <ggml.h>
#include <ggml-backend.h>
#include <ggml-cpu.h>
#include <fstream>
#include <iostream>
#include <chrono>
#include <stdexcept>
#include <vector>
#include <memory>
#include <cstring>

namespace koebridge {
namespace inference {

class InferenceEngine::Impl {
public:
    Impl() : ctx_(nullptr), model_(nullptr), initialized_(false), logits_(nullptr), vocabSize_(0),
             tokenizer_(nullptr, [](void*){}) {
        // Initialize default special tokens
        specialTokens_ = {
            {"<s>", 1},      // BOS token
            {"</s>", 2},     // EOS token
            {"<unk>", 0},    // Unknown token
            {"<pad>", -1}    // Padding token
        };
    }

    ~Impl() {
        cleanup();
    }

    bool initialize(const std::string& modelPath) {
        if (initialized_) {
            return true;
        }

        LOG_INFO("Starting model initialization...");

        // Initialize GGML context with appropriate size
        LOG_INFO("Initializing GGML context (1GB)...");
        const size_t ctxSize = 1024 * 1024 * 1024; // 1GB initial size
        ctx_ = ggml_init({ctxSize, nullptr, false});
        if (!ctx_) {
            LOG_ERROR("Failed to initialize GGML context");
            return false;
        }
        LOG_INFO("GGML context initialized successfully");

        // Load model file
        LOG_INFO("Opening model file: " + modelPath);
        std::ifstream file(modelPath, std::ios::binary);
        if (!file) {
            LOG_ERROR("Failed to open model file: " + modelPath);
            cleanup();
            return false;
        }

        try {
            // Read and validate model header
            LOG_INFO("Reading model header...");
            uint32_t magic;
            file.read(reinterpret_cast<char*>(&magic), sizeof(magic));
            if (magic != 0x67676D6C) { // "ggml" in hex
                LOG_ERROR("Invalid model file format");
                return false;
            }
            LOG_INFO("Model header validated");

            // Read model version and parameters
            LOG_INFO("Reading model architecture...");
            uint32_t version;
            file.read(reinterpret_cast<char*>(&version), sizeof(version));

            // Read model architecture parameters
            uint32_t n_layers, n_heads, n_embd, vocab_size;
            file.read(reinterpret_cast<char*>(&n_layers), sizeof(n_layers));
            file.read(reinterpret_cast<char*>(&n_heads), sizeof(n_heads));
            file.read(reinterpret_cast<char*>(&n_embd), sizeof(n_embd));
            file.read(reinterpret_cast<char*>(&vocab_size), sizeof(vocab_size));

            std::string archInfo = std::string("Model architecture:\n") +
                "  - Version: " + std::to_string(version) + "\n" +
                "  - Layers: " + std::to_string(n_layers) + "\n" +
                "  - Heads: " + std::to_string(n_heads) + "\n" +
                "  - Embedding size: " + std::to_string(n_embd) + "\n" +
                "  - Vocabulary size: " + std::to_string(vocab_size);
            LOG_INFO(archInfo);

            // Allocate model tensors
            LOG_INFO("Allocating model tensors...");
            model_ = ggml_new_tensor_3d(ctx_, GGML_TYPE_F32, n_embd, n_heads, n_layers);
            if (!model_) {
                LOG_ERROR("Failed to allocate model tensor");
                return false;
            }
            LOG_INFO("Model tensors allocated");

            // Load model weights
            LOG_INFO("Loading model weights...");
            size_t weightsSize = ggml_nbytes(model_);
            LOG_INFO("Model size: " + std::to_string(weightsSize / 1024 / 1024) + " MB");

            std::vector<uint8_t> weights(weightsSize);
            file.read(reinterpret_cast<char*>(weights.data()), weightsSize);
            if (file.gcount() != static_cast<std::streamsize>(weightsSize)) {
                LOG_ERROR("Failed to read model weights");
                return false;
            }
            LOG_INFO("Model weights loaded successfully");

            // Copy weights to tensor
            LOG_INFO("Copying weights to tensor...");
            memcpy(ggml_get_data(model_), weights.data(), weightsSize);
            LOG_INFO("Weights copied successfully");

            // Load vocabulary
            LOG_INFO("Loading vocabulary (" + std::to_string(vocab_size) + " tokens)...");
            vocab_.resize(vocab_size);
            for (size_t i = 0; i < vocab_size; ++i) {
                uint32_t tokenLength;
                file.read(reinterpret_cast<char*>(&tokenLength), sizeof(tokenLength));

                std::string token(tokenLength, '\0');
                file.read(&token[0], tokenLength);
                vocab_[i] = token;

                if (i % 10000 == 0) {
                    LOG_INFO("Loaded " + std::to_string(i) + "/" + std::to_string(vocab_size) + " tokens");
                }
            }
            LOG_INFO("Vocabulary loaded successfully");

            initialized_ = true;
            LOG_INFO("Model initialization completed successfully");
            return true;

        } catch (const std::exception& e) {
            LOG_ERROR("Error initializing model: " + std::string(e.what()));
            cleanup();
            return false;
        }
    }

    bool processInput(const std::string& input, std::string& output) {
        if (!initialized_) {
            LOG_ERROR("Engine not initialized");
            return false;
        }

        try {
            // Tokenize input
            std::vector<int> inputTokens = tokenize(input);

            // Run inference
            translation::InferenceStats stats;
            std::vector<int> outputTokens = runInference(inputTokens, translation::TranslationOptions(), stats);

            // Detokenize output
            output = detokenize(outputTokens);

            LOG_INFO("Processed input: " + std::to_string(inputTokens.size()) +
                    " tokens, " + std::to_string(stats.inferenceTimeMs) + "ms");

            return true;
        } catch (const std::exception& e) {
            LOG_ERROR("Error processing input: " + std::string(e.what()));
            return false;
        }
    }

    std::vector<int> runInference(
        const std::vector<int>& inputTokens,
        const translation::TranslationOptions& options,
        translation::InferenceStats& stats
    ) {
        auto startTime = std::chrono::high_resolution_clock::now();

        // Create input tensor
        auto inputTensor = ggml_new_tensor_1d(ctx_, GGML_TYPE_I32, inputTokens.size());
        int32_t* inputData = (int32_t*)ggml_get_data(inputTensor);
        std::copy(inputTokens.begin(), inputTokens.end(), inputData);

        // Create attention mask
        auto attentionMask = ggml_new_tensor_2d(ctx_, GGML_TYPE_F32, inputTokens.size(), inputTokens.size());
        float* maskData = (float*)ggml_get_data(attentionMask);
        std::fill(maskData, maskData + inputTokens.size() * inputTokens.size(), 1.0f);

        // Create computation graph
        auto graph = ggml_new_graph(ctx_);

        // Add model operations to graph
        auto hidden = ggml_mul_mat(ctx_, model_, inputTensor);
        auto outputTensor = ggml_soft_max(ctx_, hidden);

        // Compute graph
        ggml_build_forward_expand(graph, outputTensor);

        // Create compute plan
        int n_threads = 4;  // You might want to make this configurable
        struct ggml_cplan plan = ggml_graph_plan(graph, n_threads, nullptr);
        if (!plan.work_data) {
            plan.work_data = static_cast<uint8_t*>(malloc(plan.work_size));
        }

        if (!plan.work_data) {
            throw std::runtime_error("Failed to allocate work buffer for GGML computation");
        }

        // Compute the graph
        enum ggml_status status = ggml_graph_compute(graph, &plan);

        // Cleanup
        free(plan.work_data);

        if (status != GGML_STATUS_SUCCESS) {
            throw std::runtime_error("Failed to compute GGML graph");
        }

        // Get output tokens
        std::vector<int> outputTokens;
        float* outputData = (float*)ggml_get_data(outputTensor);
        size_t outputSize = ggml_nelements(outputTensor);

        // Convert probabilities to token IDs
        for (size_t i = 0; i < outputSize; ++i) {
            if (outputData[i] > 0.5f) {  // Simple threshold for demonstration
                outputTokens.push_back(static_cast<int>(i));
            }
        }

        // Update stats
        auto endTime = std::chrono::high_resolution_clock::now();
        stats.inferenceTimeMs = std::chrono::duration_cast<std::chrono::milliseconds>(
            endTime - startTime).count();
        stats.inputTokenCount = inputTokens.size();

        return outputTokens;
    }

    std::vector<int> tokenize(const std::string& text) {
        std::vector<int> tokens;

        // Add BOS token
        auto it = specialTokens_.find("<s>");
        if (it != specialTokens_.end()) {
            tokens.push_back(it->second);
        }

        // Simple whitespace tokenization for demonstration
        std::string word;
        for (char c : text) {
            if (std::isspace(c)) {
                if (!word.empty()) {
                    // Find token in vocabulary
                    auto it = std::find(vocab_.begin(), vocab_.end(), word);
                    if (it != vocab_.end()) {
                        tokens.push_back(std::distance(vocab_.begin(), it));
                    } else {
                        // Use unknown token
                        auto unkIt = specialTokens_.find("<unk>");
                        if (unkIt != specialTokens_.end()) {
                            tokens.push_back(unkIt->second);
                        }
                    }
                    word.clear();
                }
            } else {
                word += c;
            }
        }

        // Process last word if any
        if (!word.empty()) {
            auto it = std::find(vocab_.begin(), vocab_.end(), word);
            if (it != vocab_.end()) {
                tokens.push_back(std::distance(vocab_.begin(), it));
            } else {
                auto unkIt = specialTokens_.find("<unk>");
                if (unkIt != specialTokens_.end()) {
                    tokens.push_back(unkIt->second);
                }
            }
        }

        // Add EOS token
        it = specialTokens_.find("</s>");
        if (it != specialTokens_.end()) {
            tokens.push_back(it->second);
        }

        return tokens;
    }

    std::string detokenize(const std::vector<int>& tokens) {
        std::string text;
        bool first = true;

        for (int token : tokens) {
            // Skip special tokens
            bool isSpecial = false;
            for (const auto& pair : specialTokens_) {
                if (pair.second == token) {
                    isSpecial = true;
                    break;
                }
            }
            if (isSpecial) continue;

            // Add space between words
            if (!first) {
                text += " ";
            }
            first = false;

            // Convert token to text
            if (token >= 0 && token < static_cast<int>(vocab_.size())) {
                text += vocab_[token];
            }
        }

        return text;
    }

    void cleanup() {
        if (ctx_) {
            ggml_free(ctx_);
            ctx_ = nullptr;
        }
        model_ = nullptr;
        initialized_ = false;
    }

    bool isInitialized() const {
        return initialized_;
    }

    void* getTokenizer() {
        return tokenizer_.get();
    }

    std::vector<float> getLogits() {
        if (!initialized_ || !logits_) {
            return std::vector<float>();
        }
        return std::vector<float>(logits_, logits_ + vocabSize_);
    }

private:
    struct ggml_context* ctx_;
    struct ggml_tensor* model_;
    bool initialized_;
    std::vector<std::string> vocab_;
    std::map<std::string, int> specialTokens_;
    std::unique_ptr<void, void(*)(void*)> tokenizer_;
    float* logits_;
    size_t vocabSize_;
};

// InferenceEngine implementation
InferenceEngine::InferenceEngine() : pImpl_(std::make_unique<Impl>()) {}
InferenceEngine::~InferenceEngine() = default;

bool InferenceEngine::initialize(const std::string& modelPath) {
    return pImpl_->initialize(modelPath);
}

bool InferenceEngine::processInput(const std::string& input, std::string& output) {
    return pImpl_->processInput(input, output);
}

std::vector<int> InferenceEngine::runInference(
    const std::vector<int>& inputTokens,
    const translation::TranslationOptions& options,
    translation::InferenceStats& stats
) {
    return pImpl_->runInference(inputTokens, options, stats);
}

bool InferenceEngine::isInitialized() const {
    return pImpl_->isInitialized();
}

void* InferenceEngine::getTokenizer() {
    return pImpl_->getTokenizer();
}

std::vector<float> InferenceEngine::getLogits() {
    return pImpl_->getLogits();
}

} // namespace inference
} // namespace koebridge
