/**
 * @file model_manager.cc
 * @brief Implementation of the translation model manager
 */

#include "translation/model_manager.h"
#include "utils/config.h"
#include "utils/logger.h"
#include "models/ggml_model.h"
#include <QDir>
#include <QFileInfo>
#include <QStandardPaths>
#include <iostream>

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

    // Find the model in the available models list
    for (const auto& model : models_) {
        if (model.id == modelId) {
            // Unload current model if any
            if (modelLoaded_) {
                unloadModel();
            }

            // TODO: Implement actual model loading with GGMLModel
            // For now, just update the model info
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
    translationModel_.reset();
    modelLoaded_ = false;
    activeModel_ = ModelInfo{};
}

std::shared_ptr<ITranslationModel> ModelManager::getTranslationModel() {
    if (!modelLoaded_) {
        return nullptr;
    }
    
    // TODO: Return the actual translation model when implemented
    // For now, just return null to avoid undefined symbols
    return nullptr;
}

std::string ModelManager::getModelPathFromConfig() const {
    // TODO: Use actual config service instead of hardcoded paths
    
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
        std::cerr << "Model directory does not exist: " << modelPath_ << std::endl;
        return;
    }
    
    // Scan directory for model files
    const auto entries = dir.entryInfoList(QDir::Files | QDir::Readable, QDir::Name);
    std::cerr << "Found " << entries.size() << " files in model directory" << std::endl;
    
    for (const auto& entry : entries) {
        // Filter for model files
        if (entry.suffix() == "bin" || entry.suffix() == "ggml" || entry.suffix() == "gguf") {
            ModelInfo info;
            info.id = entry.baseName().toStdString();
            info.path = entry.filePath().toStdString();
            info.size = entry.size();
            info.lastModified = static_cast<std::time_t>(entry.lastModified().toSecsSinceEpoch());
            
            // Set model capabilities based on file name
            if (info.id.find("nllb") != std::string::npos) {
                info.capabilities.push_back("translation");
                info.modelType = "nllb";
                info.description = "NLLB-200 model for multilingual translation";
                
                // Detect language pair from filename (e.g., nllb-ja-en for Japanese to English)
                size_t langPos = info.id.find("-", info.id.find("nllb") + 4);
                if (langPos != std::string::npos) {
                    std::string langs = info.id.substr(langPos + 1);
                    size_t sepPos = langs.find("-");
                    if (sepPos != std::string::npos) {
                        std::string srcLang = langs.substr(0, sepPos);
                        std::string tgtLang = langs.substr(sepPos + 1);
                        
                        // Map short language codes to NLLB language codes
                        if (srcLang == "ja") info.sourceLanguage = "jpn_Jpan";
                        else if (srcLang == "en") info.sourceLanguage = "eng_Latn";
                        else if (srcLang == "zh") info.sourceLanguage = "zho_Hans";
                        else info.sourceLanguage = srcLang;
                        
                        if (tgtLang == "ja") info.targetLanguage = "jpn_Jpan";
                        else if (tgtLang == "en") info.targetLanguage = "eng_Latn";
                        else if (tgtLang == "zh") info.targetLanguage = "zho_Hans";
                        else info.targetLanguage = tgtLang;
                    }
                }
            } else {
                // Generic model capabilities for other model types
                info.capabilities.push_back("text-generation");
                info.modelType = "general";
            }
            
            models_.push_back(info);
            std::cerr << "Added model: " << info.id << " at " << info.path 
                      << " (Type: " << info.modelType << ")" << std::endl;
        }
    }
    
    std::cerr << "Total models found: " << models_.size() << std::endl;
}

} // namespace translation
} // namespace koebridge
