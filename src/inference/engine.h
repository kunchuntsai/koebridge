/**
 * @file engine.h
 * @brief Header file for the inference engine that handles model inference
 */

#pragma once

#include <string>
#include <vector>
#include <memory>
#include "translation/data_structures.h"
#include "utils/config.h"

namespace koebridge {
namespace inference {

struct InferenceStats {
    double inferenceTimeMs;
    size_t tokensProcessed;
    size_t memoryUsed;
};

/**
 * @class InferenceEngine
 * @brief Core inference engine for running translation models
 *
 * This class provides the core functionality for running inference on translation models.
 * It handles model initialization, tokenization, and inference operations.
 */
class InferenceEngine {
public:
    /**
     * @brief Constructor for InferenceEngine
     */
    InferenceEngine();

    /**
     * @brief Destructor for InferenceEngine
     */
    ~InferenceEngine();

    /**
     * @brief Initialize the inference engine with a model
     * @param modelPath Path to the model file
     * @return bool True if initialization was successful, false otherwise
     */
    bool initialize(const std::string& modelPath);

    /**
     * @brief Process input text and generate output text
     * @param input Input text to process
     * @param output Output text generated from the input
     * @return bool True if processing was successful, false otherwise
     */
    bool processInput(const std::string& input, std::string& output);

    /**
     * @brief Check if the engine is initialized
     * @return bool True if the engine is initialized and ready for inference
     */
    bool isInitialized() const;

    /**
     * @brief Run inference on input tokens
     * @param inputTokens Vector of input token IDs
     * @param options Translation options for inference
     * @param stats Statistics about the inference operation
     * @return std::vector<int> Vector of output token IDs
     */
    std::vector<int> runInference(
        const std::vector<int>& inputTokens,
        const translation::TranslationOptions& options,
        translation::InferenceStats& stats
    );

    /**
     * @brief Convert text to token IDs
     * @param text Input text to tokenize
     * @return std::vector<int> Vector of token IDs
     */
    std::vector<int> tokenize(const std::string& text);

    /**
     * @brief Convert token IDs back to text
     * @param tokens Vector of token IDs to detokenize
     * @return std::string The detokenized text
     */
    std::string detokenize(const std::vector<int>& tokens);

    /**
     * @brief Get the tokenizer instance
     * @return void* Pointer to the tokenizer instance
     */
    void* getTokenizer();

    /**
     * @brief Get the logits from the last inference
     * @return std::vector<float> Vector of logits from the last inference
     */
    std::vector<float> getLogits();

private:
    class Impl;
    std::unique_ptr<Impl> pImpl_;
};

} // namespace inference
} // namespace koebridge
