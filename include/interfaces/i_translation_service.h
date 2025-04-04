/**
 * @file i_translation_service.h
 * @brief Interface definition for translation services
 */

#pragma once

#include <string>
#include <functional>
#include "translation/data_structures.h"

using TranslationCallback = std::function<void(const TranslationResult&)>;

/**
 * @class ITranslationService
 * @brief Abstract interface for translation services
 * 
 * This interface defines the contract for translation services that handle
 * text translation operations. It provides methods for service lifecycle,
 * translation operations, and configuration management.
 */
class ITranslationService {
public:
    /**
     * @brief Virtual destructor
     */
    virtual ~ITranslationService() = default;
    
    /**
     * @brief Initialize the translation service
     * @return bool True if initialization was successful, false otherwise
     */
    virtual bool initialize() = 0;
    
    /**
     * @brief Shutdown the translation service
     */
    virtual void shutdown() = 0;
    
    /**
     * @brief Translate text synchronously
     * @param japaneseText The Japanese text to translate
     * @return std::string The translated text
     */
    virtual std::string translateText(const std::string& japaneseText) = 0;
    
    /**
     * @brief Translate text asynchronously
     * @param japaneseText The Japanese text to translate
     * @param callback Callback function to be called when translation is complete
     */
    virtual void translateTextAsync(const std::string& japaneseText, TranslationCallback callback) = 0;
    
    /**
     * @brief Set translation options
     * @param options The translation options to apply
     */
    virtual void setTranslationOptions(const TranslationOptions& options) = 0;
    
    /**
     * @brief Get current translation options
     * @return TranslationOptions The current translation options
     */
    virtual TranslationOptions getTranslationOptions() const = 0;
    
    // Signals are implemented via Qt signals/slots in derived class
};
