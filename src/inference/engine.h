/**
 * @file engine.h
 * @brief Header file for the inference engine that handles model inference
 */

#pragma once

#include <string>
#include <vector>
#include <memory>
#include "translation/data_structures.h"

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
     * @param config Configuration parameters for the model
     * @return bool True if initialization was successful, false otherwise
     */
    bool initialize(const std::string& modelPath, const std::map<std::string, std::string>& config);
    
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
        const TranslationOptions& options, 
        InferenceStats& stats
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
    
private:
    class Impl;
    std::unique_ptr<Impl> pImpl_;
    bool initialized_ = false;
};
