#pragma once

#include <string>
#include <vector>
#include <memory>
#include <functional>
#include "i_translation_model.h"

// Forward declarations
struct ModelInfo;
using ProgressCallback = std::function<void(int, const std::string&)>;

class IModelManager {
public:
    virtual ~IModelManager() = default;
    
    // Model discovery
    virtual std::vector<ModelInfo> getAvailableModels() const = 0;
    virtual ModelInfo getActiveModel() const = 0;
    
    // Model lifecycle
    virtual bool loadModel(const std::string& modelId) = 0;
    virtual void unloadCurrentModel() = 0;
    virtual bool isModelLoaded() const = 0;
    
    // Model operations
    virtual std::shared_ptr<ITranslationModel> getTranslationModel() = 0;
    virtual bool downloadModel(const std::string& modelId, ProgressCallback callback) = 0;
};
