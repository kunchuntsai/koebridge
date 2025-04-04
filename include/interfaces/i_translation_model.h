#pragma once

#include <string>
#include <future>
#include "translation/data_structures.h"

class ITranslationModel {
public:
    virtual ~ITranslationModel() = default;
    
    // Initialization
    virtual bool initialize() = 0;
    virtual bool isInitialized() const = 0;
    
    // Core operations
    virtual TranslationResult translate(const std::string& text, const TranslationOptions& options) = 0;
    virtual std::future<TranslationResult> translateAsync(const std::string& text, const TranslationOptions& options) = 0;
    
    // Status and metadata
    virtual ModelInfo getModelInfo() const = 0;
    virtual InferenceStats getLastInferenceStats() const = 0;
};
