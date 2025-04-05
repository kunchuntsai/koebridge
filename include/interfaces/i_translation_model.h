/**
 * @file i_translation_model.h
 * @brief Interface definition for translation models
 */

#pragma once

#include <string>
#include <future>
#include "../src/translation/data_structures.h"

namespace koebridge {
namespace translation {

/**
 * @class ITranslationModel
 * @brief Abstract interface for translation models
 * 
 * This interface defines the contract that all translation models must implement.
 * It provides methods for initialization, translation operations, and status reporting.
 */
class ITranslationModel {
public:
    /**
     * @brief Virtual destructor
     */
    virtual ~ITranslationModel() = default;
    
    /**
     * @brief Initialize the translation model
     * @return bool True if initialization was successful, false otherwise
     */
    virtual bool initialize() = 0;
    
    /**
     * @brief Check if the model is initialized
     * @return bool True if the model is initialized and ready for translation
     */
    virtual bool isInitialized() const = 0;
    
    /**
     * @brief Translate text synchronously
     * @param text The input text to translate
     * @param options Translation options and parameters
     * @return TranslationResult The translation result containing the translated text and metadata
     */
    virtual TranslationResult translate(const std::string& text, const TranslationOptions& options) = 0;
    
    /**
     * @brief Translate text asynchronously
     * @param text The input text to translate
     * @param options Translation options and parameters
     * @return std::future<TranslationResult> Future containing the translation result
     */
    virtual std::future<TranslationResult> translateAsync(const std::string& text, const TranslationOptions& options) = 0;
    
    /**
     * @brief Get information about the loaded model
     * @return ModelInfo Information about the model including name, version, and type
     */
    virtual ModelInfo getModelInfo() const = 0;
    
    /**
     * @brief Get statistics from the last inference operation
     * @return InferenceStats Statistics about the last translation operation
     */
    virtual InferenceStats getLastInferenceStats() const = 0;
};

} // namespace translation
} // namespace koebridge
