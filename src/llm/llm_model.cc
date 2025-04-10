/**
 * @file llm_model.cc
 * @brief Implementation of the LLM model
 */

#include "llm/llm_model.h"
#include "utils/config.h"
#include "utils/logger.h"
#include <iostream>
#include <future>
#include <algorithm>
#include <chrono>

namespace koebridge {
namespace llm {

LLMModel::LLMModel(const translation::ModelInfo& modelInfo, const LLMConfig& config)
    : GGMLModel(modelInfo), config_(config) {
    // Additional initialization for LLM-specific parameters will be done in initialize()
}

LLMModel::~LLMModel() {
    // Cleanup will be handled by the base class
}

bool LLMModel::initialize() {
    // First call the base class implementation
    if (!GGMLModel::initialize()) {
        std::cerr << "Failed to initialize base GGML model" << std::endl;
        return false;
    }

    // TODO: Load LLM-specific parameters and configure the model
    // - Set up the context size
    // - Configure sampling parameters
    // - Set up device-specific optimizations (GPU, etc)
    // - Load additional resources like special tokens

    std::cout << "LLM model initialized with config: "
              << "context size=" << config_.contextSize
              << ", temperature=" << config_.temperature
              << ", threads=" << config_.numThreads << std::endl;

    return true;
}

LLMOutput LLMModel::complete(const std::string& prompt) {
    LLMOutput output;
    translation::InferenceStats stats;

    try {
        // Format the prompt for the model
        std::string formattedPrompt = formatPrompt(prompt);

        // Run the generation
        std::vector<int> outputTokens;
        if (runGeneration(formattedPrompt, outputTokens, stats)) {
            // Convert tokens back to text using our local method
            output.text = localDetokenize(outputTokens);
            output.success = true;
            output.stats = stats;
        } else {
            output.success = false;
            output.errorMessage = "Generation failed";
        }
    } catch (const std::exception& e) {
        output.success = false;
        output.errorMessage = std::string("Error during generation: ") + e.what();
    }

    return output;
}

std::future<LLMOutput> LLMModel::completeAsync(const std::string& prompt) {
    // TODO: Implement with proper thread pool management
    return std::async(std::launch::async, [this, prompt]() {
        return complete(prompt);
    });
}

void LLMModel::setConfig(const LLMConfig& config) {
    config_ = config;
}

LLMConfig LLMModel::getConfig() const {
    return config_;
}

bool LLMModel::runGeneration(const std::string& prompt, std::vector<int>& output, translation::InferenceStats& stats) {
    if (!isInitialized()) {
        std::cerr << "LLM model not initialized" << std::endl;
        return false;
    }

    auto startTime = std::chrono::high_resolution_clock::now();

    // Tokenize the prompt
    std::vector<int> promptTokens = localTokenize(prompt);
    stats.inputTokenCount = promptTokens.size();

    // Set up the context window
    size_t maxContextSize = std::min(config_.contextSize, static_cast<int>(promptTokens.size() + config_.maxLength));
    std::vector<int> contextTokens(promptTokens.begin(), promptTokens.begin() + maxContextSize);

    // Initialize output vector with context
    output = contextTokens;

    // Run inference through GGML
    auto inferenceStartTime = std::chrono::high_resolution_clock::now();

    // Generate tokens one by one
    for (int i = 0; i < config_.maxLength; ++i) {
        // Get next token probabilities
        std::vector<float> logits = engine_->getLogits();

        // Apply temperature
        if (config_.temperature > 0) {
            for (float& logit : logits) {
                logit /= config_.temperature;
            }
        }

        // Apply top-k filtering
        if (config_.topK > 0) {
            std::vector<std::pair<float, int>> logitIndexPairs;
            for (size_t j = 0; j < logits.size(); ++j) {
                logitIndexPairs.emplace_back(logits[j], j);
            }
            std::partial_sort(logitIndexPairs.begin(),
                            logitIndexPairs.begin() + config_.topK,
                            logitIndexPairs.end(),
                            std::greater<>());

            // Zero out probabilities for tokens not in top-k
            for (size_t j = config_.topK; j < logitIndexPairs.size(); ++j) {
                logits[logitIndexPairs[j].second] = -INFINITY;
            }
        }

        // Apply top-p (nucleus) sampling
        if (config_.topP < 1.0f) {
            std::vector<float> sortedLogits = logits;
            std::sort(sortedLogits.begin(), sortedLogits.end(), std::greater<>());

            float cumulativeProb = 0.0f;
            float threshold = -INFINITY;

            for (float logit : sortedLogits) {
                float prob = std::exp(logit);
                cumulativeProb += prob;
                if (cumulativeProb >= config_.topP) {
                    threshold = logit;
                    break;
                }
            }

            for (float& logit : logits) {
                if (logit < threshold) {
                    logit = -INFINITY;
                }
            }
        }

        // Apply repeat penalty
        if (config_.repeatPenalty > 1.0f) {
            for (int token : output) {
                logits[token] /= config_.repeatPenalty;
            }
        }

        // Sample next token
        float sum = 0.0f;
        for (float logit : logits) {
            sum += std::exp(logit);
        }

        float r = static_cast<float>(rand()) / RAND_MAX * sum;
        float cumsum = 0.0f;
        int nextToken = -1;

        for (size_t j = 0; j < logits.size(); ++j) {
            cumsum += std::exp(logits[j]);
            if (cumsum > r) {
                nextToken = j;
                break;
            }
        }

        if (nextToken == -1) {
            nextToken = logits.size() - 1;
        }

        // Add token to output
        output.push_back(nextToken);

        // Check for end of sequence
        if (nextToken == 2) { // EOS token
            break;
        }
    }

    auto inferenceEndTime = std::chrono::high_resolution_clock::now();
    auto endTime = std::chrono::high_resolution_clock::now();

    // Update statistics
    stats.outputTokenCount = output.size() - promptTokens.size();
    stats.totalTimeMs = std::chrono::duration<float, std::milli>(endTime - startTime).count();
    stats.inferenceTimeMs = std::chrono::duration<float, std::milli>(inferenceEndTime - inferenceStartTime).count();

    return true;
}

std::string LLMModel::formatPrompt(const std::string& prompt) {
    std::string modelType = modelInfo_.modelType;

    if (modelType == "chatml") {
        return "<|im_start|>user\n" + prompt + "<|im_end|>\n<|im_start|>assistant\n";
    } else if (modelType == "alpaca") {
        return "### Instruction:\n" + prompt + "\n\n### Response:\n";
    } else if (modelType == "llama") {
        return "[INST] " + prompt + " [/INST]";
    } else {
        // Default format
        return prompt + "\n";
    }
}

std::vector<int> LLMModel::localTokenize(const std::string& text) {
    std::vector<int> tokens;

    // Add BOS token
    tokens.push_back(1);  // BOS token ID

    // Get the tokenizer from the engine
    auto* tokenizer = static_cast<sentencepiece::SentencePieceProcessor*>(engine_->getTokenizer());
    if (!tokenizer) {
        std::cerr << "Tokenizer not available" << std::endl;
        return tokens;
    }

    // Tokenize the text
    std::vector<int> textTokens;
    tokenizer->Encode(text, &textTokens);
    tokens.insert(tokens.end(), textTokens.begin(), textTokens.end());

    // Add EOS token
    tokens.push_back(2);  // EOS token ID

    return tokens;
}

std::string LLMModel::localDetokenize(const std::vector<int>& tokens) {
    // Get the tokenizer from the engine
    auto* tokenizer = static_cast<sentencepiece::SentencePieceProcessor*>(engine_->getTokenizer());
    if (!tokenizer) {
        std::cerr << "Tokenizer not available" << std::endl;
        return "";
    }

    // Remove special tokens (BOS and EOS)
    std::vector<int> textTokens;
    if (tokens.size() > 2) {
        textTokens.assign(tokens.begin() + 1, tokens.end() - 1);
    }

    // Detokenize
    std::string text;
    tokenizer->Decode(textTokens, &text);
    return text;
}

} // namespace llm
} // namespace koebridge