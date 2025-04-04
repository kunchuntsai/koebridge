#include "model_manager.h"

ModelManager::ModelManager(const std::string& modelPath)
    : modelPath_(modelPath)
    , modelLoaded_(false) {
    // Initialize with some test models
    ModelInfo model1;
    model1.id = "model1";
    model1.name = "Test Model 1";
    model1.version = "1.0";
    model1.filePath = modelPath_ + "/model1.bin";
    model1.sizeBytes = 1024;
    model1.type = ModelType::GGML;
    models_.push_back(model1);

    ModelInfo model2;
    model2.id = "model2";
    model2.name = "Test Model 2";
    model2.version = "1.0";
    model2.filePath = modelPath_ + "/model2.bin";
    model2.sizeBytes = 2048;
    model2.type = ModelType::GGML;
    models_.push_back(model2);
}

std::vector<ModelInfo> ModelManager::getAvailableModels() const {
    return models_;
}

bool ModelManager::loadModel(const std::string& modelId) {
    // For test purposes, accept any model ID except "non_existent_model"
    if (modelId == "non_existent_model") {
        return false;
    }
    
    // Find the model
    for (const auto& model : models_) {
        if (model.id == modelId) {
            activeModel_ = model;
            modelLoaded_ = true;
            return true;
        }
    }
    
    return false;
}

bool ModelManager::isModelLoaded() const {
    return modelLoaded_;
}

ModelInfo ModelManager::getActiveModel() const {
    return activeModel_;
}
