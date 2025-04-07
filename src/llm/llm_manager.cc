/**
 * @file llm_manager.cc
 * @brief Implementation of the LLM model manager
 */

#include "llm/llm_manager.h"
#include "utils/config.h"
#include "utils/logger.h"
#include "llm/nllb_model.h"
#include <iostream>

namespace koebridge {
namespace llm {

LLMManager::LLMManager(std::shared_ptr<translation::ModelManager> modelManager)
    : modelManager_(modelManager), initialized_(false) {
}

LLMManager::~LLMManager() {
    unloadModel();
}

bool LLMManager::initialize() {
    if (initialized_) {
        return true;
    }
    
    if (!modelManager_) {
        std::cerr << "Model manager not provided" << std::endl;
        return false;
    }
    
    // Ensure the model manager is initialized
    if (!modelManager_->initialize()) {
        std::cerr << "Failed to initialize model manager" << std::endl;
        return false;
    }
    
    std::cout << "LLM manager initialized" << std::endl;
    initialized_ = true;
    return true;
}

bool LLMManager::loadModel(const std::string& modelId, const LLMConfig& config) {
    if (!initialized_) {
        std::cerr << "LLM manager not initialized" << std::endl;
        return false;
    }
    
    // First, have the model manager load the model
    if (!modelManager_->loadModel(modelId)) {
        std::cerr << "Failed to load model: " << modelId << std::endl;
        return false;
    }
    
    // Get the model info
    activeModel_ = modelManager_->getActiveModel();
    config_ = config;
    
    // Create the LLM model
    model_ = createModel(activeModel_, config_);
    if (!model_) {
        std::cerr << "Failed to create LLM model: " << modelId << std::endl;
        return false;
    }
    
    // Initialize the model
    if (!model_->initialize()) {
        std::cerr << "Failed to initialize LLM model: " << modelId << std::endl;
        model_.reset();
        return false;
    }
    
    std::cout << "Loaded LLM model: " << modelId << std::endl;
    return true;
}

bool LLMManager::unloadModel() {
    model_.reset();
    return true;
}

bool LLMManager::isModelLoaded() const {
    return model_ != nullptr && model_->isInitialized();
}

translation::ModelInfo LLMManager::getActiveModel() const {
    return activeModel_;
}

std::shared_ptr<LLMModel> LLMManager::getModel() {
    return model_;
}

LLMOutput LLMManager::complete(const std::string& prompt) {
    LLMOutput output;
    
    if (!isModelLoaded()) {
        output.success = false;
        output.errorMessage = "No LLM model loaded";
        return output;
    }
    
    try {
        output = model_->complete(prompt);
    } catch (const std::exception& e) {
        output.success = false;
        output.errorMessage = std::string("Error during completion: ") + e.what();
    }
    
    return output;
}

std::future<LLMOutput> LLMManager::completeAsync(const std::string& prompt) {
    if (!isModelLoaded()) {
        auto future = std::async(std::launch::deferred, []() {
            LLMOutput output;
            output.success = false;
            output.errorMessage = "No LLM model loaded";
            return output;
        });
        return future;
    }
    
    return model_->completeAsync(prompt);
}

void LLMManager::setConfig(const LLMConfig& config) {
    config_ = config;
    if (model_) {
        model_->setConfig(config);
    }
}

LLMConfig LLMManager::getConfig() const {
    return config_;
}

std::shared_ptr<LLMModel> LLMManager::createModel(const translation::ModelInfo& modelInfo, const LLMConfig& config) {
    // Determine which model type to create based on the model file properties
    
    // Default language settings
    std::string sourceLanguage = "jpn_Jpan"; // Japanese
    std::string targetLanguage = "eng_Latn"; // English
    
    // Check config for language settings
    Config& configInstance = Config::getInstance();
    sourceLanguage = configInstance.getString("translation.source_language", sourceLanguage);
    targetLanguage = configInstance.getString("translation.target_language", targetLanguage);
    
    // Check if this is an NLLB model based on filename
    if (modelInfo.id.find("nllb") != std::string::npos) {
        std::cout << "Creating NLLB model with languages: " << sourceLanguage 
                  << " -> " << targetLanguage << std::endl;
        try {
            return std::make_shared<NLLBModel>(modelInfo, sourceLanguage, targetLanguage, config);
        } catch (const std::exception& e) {
            std::cerr << "Error creating NLLB model: " << e.what() << std::endl;
            return nullptr;
        }
    }
    
    // For all other model types, create a basic LLMModel
    try {
        return std::make_shared<LLMModel>(modelInfo, config);
    } catch (const std::exception& e) {
        std::cerr << "Error creating LLM model: " << e.what() << std::endl;
        return nullptr;
    }
}

} // namespace llm
} // namespace koebridge 