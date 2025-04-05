/**
 * @file model_manager.cc
 * @brief Implementation of the translation model manager
 */

#include "translation/model_manager.h"
#include "utils/config.h"
#include "utils/logger.h"
#include <QDir>
#include <QFileInfo>
#include <QStandardPaths>

namespace koebridge {
namespace translation {

ModelManager::ModelManager(const std::string& modelPath) 
    : modelPath_(modelPath.empty() ? getModelPathFromConfig() : modelPath)
    , modelLoaded_(false) {
}

ModelManager::~ModelManager() {
    unloadCurrentModel();
}

bool ModelManager::initialize() {
    // Scan for available models
    scanForModels();
    return true;
}

std::vector<ModelInfo> ModelManager::getAvailableModels() const {
    return models_;
}

ModelInfo ModelManager::getActiveModel() const {
    return activeModel_;
}

bool ModelManager::loadModel(const std::string& modelId) {
    // Check if this model is already loaded
    if (modelLoaded_ && activeModel_.id == modelId) {
        return true;
    }
    
    // Unload current model if any
    unloadCurrentModel();
    
    // Find the model in the available models list
    for (const auto& model : models_) {
        if (model.id == modelId) {
            // TODO: Implement actual model loading logic
            
            // For now, just update the active model
            activeModel_ = model;
            modelLoaded_ = true;
            return true;
        }
    }
    
    return false;
}

bool ModelManager::unloadModel() {
    if (!modelLoaded_) {
        return true;  // Nothing to unload
    }
    
    unloadCurrentModel();
    return true;
}

bool ModelManager::isModelLoaded() const {
    return modelLoaded_;
}

bool ModelManager::downloadModel(const std::string& modelId, ProgressCallback callback) {
    // TODO: Implement model download functionality
    // This is just a placeholder
    if (callback) {
        callback(0, "Starting download for model: " + modelId);
        callback(50, "Downloading model: " + modelId);
        callback(100, "Completed download for model: " + modelId);
    }
    
    // Rescan for models to include the newly downloaded one
    scanForModels();
    
    return true;
}

void ModelManager::unloadCurrentModel() {
    // Reset the state
    modelLoaded_ = false;
    activeModel_ = ModelInfo{};
}

std::shared_ptr<ITranslationModel> ModelManager::getTranslationModel() {
    if (!modelLoaded_) {
        return nullptr;
    }
    
    // TODO: Return the actual translation model
    return nullptr;
}

std::string ModelManager::getModelPathFromConfig() const {
    // Try to get path from config
    // TODO: Use actual config service
    
    // If not specified, use default location
    QString dataPath = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    QString modelDirPath = dataPath + "/models";
    
    // Create directory if it doesn't exist
    QDir dir(modelDirPath);
    if (!dir.exists()) {
        dir.mkpath(".");
    }
    
    return modelDirPath.toStdString();
}

void ModelManager::scanForModels() {
    models_.clear();
    
    QDir dir(QString::fromStdString(modelPath_));
    if (!dir.exists()) {
        return;
    }
    
    // Scan directory for model files
    const auto entries = dir.entryInfoList(QDir::Files | QDir::Readable, QDir::Name);
    for (const auto& entry : entries) {
        // Filter for model files (simple check for now)
        if (entry.suffix() == "bin" || entry.suffix() == "ggml") {
            ModelInfo info;
            info.id = entry.baseName().toStdString();
            info.path = entry.filePath().toStdString();
            info.size = entry.size();
            info.lastModified = static_cast<std::time_t>(entry.lastModified().toSecsSinceEpoch());
            
            models_.push_back(info);
        }
    }
}

} // namespace translation
} // namespace koebridge
