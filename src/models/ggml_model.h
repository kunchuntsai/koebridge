/**
 * @file ggml_model.h
 * @brief Header file for the GGML model implementation
 */

#pragma once

#include <memory>
#include <string>
#include <vector>
#include <thread>
#include "../../include/interfaces/i_translation_model.h"
#include "../translation/data_structures.h"

class ThreadPool;
class InferenceEngine;

/**
 * @class GGMLModel
 * @brief Implementation of ITranslationModel for GGML format models
 * 
 * This class provides a concrete implementation of the ITranslationModel interface
 * specifically for GGML format models, handling model loading, inference, and
 * asynchronous translation operations.
 */
class GGMLModel : public ITranslationModel {
public:
    /**
     * @brief Constructor for GGMLModel
     * @param modelInfo Information about the model to load
     */
    GGMLModel(const ModelInfo& modelInfo);
    
    /**
     * @brief Destructor for GGMLModel
     */
    ~GGMLModel() override;
    
    /**
     * @brief Initialize the GGML model
     * @return bool True if initialization was successful, false otherwise
     */
    bool initialize() override;
    
    /**
     * @brief Check if the model is initialized
     * @return bool True if the model is initialized and ready for translation
     */
    bool isInitialized() const override;
    
    /**
     * @brief Translate text synchronously
     * @param text The input text to translate
     * @param options Translation options and parameters
     * @return TranslationResult The translation result
     */
    TranslationResult translate(const std::string& text, const TranslationOptions& options) override;
    
    /**
     * @brief Translate text asynchronously
     * @param text The input text to translate
     * @param options Translation options and parameters
     * @return std::future<TranslationResult> Future containing the translation result
     */
    std::future<TranslationResult> translateAsync(const std::string& text, const TranslationOptions& options) override;
    
    /**
     * @brief Get information about the loaded model
     * @return ModelInfo Information about the model
     */
    ModelInfo getModelInfo() const override;
    
    /**
     * @brief Get statistics from the last inference operation
     * @return InferenceStats Statistics about the last translation operation
     */
    InferenceStats getLastInferenceStats() const override;

private:
    ModelInfo modelInfo_;                    ///< Information about the loaded model
    bool initialized_ = false;               ///< Model initialization flag
    InferenceStats lastStats_;               ///< Statistics from last inference
    
    std::unique_ptr<InferenceEngine> engine_; ///< Inference engine for model operations
    std::shared_ptr<ThreadPool> threadPool_;  ///< Thread pool for async operations
    
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
     * @brief Run inference on input tokens
     * @param inputTokens Vector of input token IDs
     * @param outputTokens Vector to store output token IDs
     * @param options Translation options for inference
     */
    void runInference(const std::vector<int>& inputTokens, std::vector<int>& outputTokens, const TranslationOptions& options);
};
