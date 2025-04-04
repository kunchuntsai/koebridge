#pragma once

#include <string>
#include <functional>
#include "translation/data_structures.h"

using TranslationCallback = std::function<void(const TranslationResult&)>;

class ITranslationService {
public:
    virtual ~ITranslationService() = default;
    
    // Service lifecycle
    virtual bool initialize() = 0;
    virtual void shutdown() = 0;
    
    // Translation operations
    virtual std::string translateText(const std::string& japaneseText) = 0;
    virtual void translateTextAsync(const std::string& japaneseText, TranslationCallback callback) = 0;
    
    // Configuration
    virtual void setTranslationOptions(const TranslationOptions& options) = 0;
    virtual TranslationOptions getTranslationOptions() const = 0;
    
    // Signals are implemented via Qt signals/slots in derived class
};
