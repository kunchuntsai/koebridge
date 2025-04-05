/**
 * @file ggml_model.h
 * @brief Header file for the GGML model implementation
 */

#pragma once

#include <string>
#include <memory>
#include <future>
#include <vector>
#include <thread>
#include "inference/engine.h"
#include "interfaces/i_translation_model.h"
#include "translation/data_structures.h"

namespace koebridge {
namespace models {

class ThreadPool;

/**
 * @class GGMLModel
 * @brief Implementation of ITranslationModel for GGML format models
 * 
 * This class provides a concrete implementation of the ITranslationModel interface
 * specifically for GGML format models, handling model loading, inference, and
 * asynchronous translation operations.
 */
class GGMLModel : public translation::ITranslationModel {
public:
    /**
     * @brief Constructor for GGMLModel
     * @param modelInfo Information about the model to load
     */
    explicit GGMLModel(const translation::ModelInfo& modelInfo);
    
    /**
     * @brief Destructor for GGMLModel
     */
    ~GGMLModel();
    
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
    translation::TranslationResult translate(const std::string& text, const translation::TranslationOptions& options) override;
    
    /**
     * @brief Translate text asynchronously
     * @param text The input text to translate
     * @param options Translation options and parameters
     * @return std::future<TranslationResult> Future containing the translation result
     */
    std::future<translation::TranslationResult> translateAsync(const std::string& text, const translation::TranslationOptions& options) override;
    
    /**
     * @brief Get information about the loaded model
     * @return ModelInfo Information about the model
     */
    translation::ModelInfo getModelInfo() const override;
    
    /**
     * @brief Get statistics from the last inference operation
     * @return InferenceStats Statistics about the last translation operation
     */
    translation::InferenceStats getLastInferenceStats() const override;

private:
    translation::ModelInfo modelInfo_;                    ///< Information about the loaded model
    bool initialized_ = false;               ///< Model initialization flag
    translation::InferenceStats lastStats_;               ///< Statistics from last inference
    
    std::unique_ptr<inference::InferenceEngine> engine_; ///< Inference engine for model operations
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
    void runInference(const std::vector<int>& inputTokens, std::vector<int>& outputTokens, const translation::TranslationOptions& options);
};

} // namespace models
} // namespace koebridge
