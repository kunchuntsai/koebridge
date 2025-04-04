/**
 * @file model_manager.cc
 * @brief Implementation of the ModelManager class for managing translation models
 */

#include "model_manager.h"
#include <filesystem>
#include <fstream>
#include <iostream>
#include <thread>
#include <chrono>
#include <sstream>
#include <cstdlib>
#include <sys/stat.h>
#include <unistd.h>

namespace fs = std::filesystem;

// Error messages
namespace {
    constexpr const char* ERROR_MODEL_NOT_FOUND = "Model not found: ";
    constexpr const char* ERROR_FILE_NOT_FOUND = "Model file not found: ";
    constexpr const char* ERROR_FILE_CREATE = "Failed to create model file: ";
    constexpr const char* ERROR_MODEL_INIT = "Failed to initialize model: ";
    constexpr const char* ERROR_SCAN_DIR = "Error scanning models directory: ";
    constexpr const char* ERROR_INVALID_MODEL = "Invalid model file: ";
    constexpr const char* ERROR_DOWNLOAD_FAILED = "Failed to download model: ";
    constexpr const char* ERROR_INVALID_PATH = "Invalid model path: ";
    constexpr const char* ERROR_PATH_CREATE = "Failed to create model directory: ";
    constexpr const char* ERROR_PATH_PERMISSION = "Directory not writable: ";
}

std::string ModelManager::getModelPathFromConfig() const {
    try {
        // Get path from config with default value
        return Config::getInstance().getPath("model_path", "~/.koebridge/models");
    } catch (const std::exception& e) {
        std::cerr << "Error getting model path from config: " << e.what() << std::endl;
        // Fallback to default path
        const char* home = std::getenv("HOME");
        if (!home) {
            throw std::runtime_error("HOME environment variable not set");
        }
        return std::string(home) + "/.koebridge/models";
    }
}

ModelManager::ModelManager(const std::string& modelPath)
    : modelPath_(modelPath.empty() ? getModelPathFromConfig() : modelPath)
    , modelLoaded_(false) {
    try {
        // Create model directory if it doesn't exist
        if (!fs::exists(modelPath_)) {
            if (!fs::create_directories(modelPath_)) {
                std::stringstream ss;
                ss << ERROR_PATH_CREATE << modelPath_;
                throw std::runtime_error(ss.str());
            }
        }
        
        // Verify directory is accessible
        if (!fs::is_directory(modelPath_)) {
            std::stringstream ss;
            ss << ERROR_INVALID_PATH << modelPath_ << " (not a directory)";
            throw std::runtime_error(ss.str());
        }
        
        // Check write permissions using POSIX functions
        struct stat st;
        if (stat(modelPath_.c_str(), &st) != 0) {
            std::stringstream ss;
            ss << ERROR_INVALID_PATH << modelPath_ << " (cannot access)";
            throw std::runtime_error(ss.str());
        }
        
        // Check if directory is writable by current user
        if (access(modelPath_.c_str(), W_OK) != 0) {
            std::stringstream ss;
            ss << ERROR_PATH_PERMISSION << modelPath_;
            throw std::runtime_error(ss.str());
        }
        
        // Scan for available models
        scanForModels();
    } catch (const std::exception& e) {
        std::cerr << "Error initializing ModelManager: " << e.what() << std::endl;
        throw;
    }
}

void ModelManager::scanForModels() {
    models_.clear();
    
    try {
        for (const auto& entry : fs::directory_iterator(modelPath_)) {
            if (entry.path().extension() == ".bin") {
                try {
                    ModelInfo model;
                    model.id = entry.path().stem().string();
                    model.name = model.id; // Use filename as name for now
                    model.version = "1.0"; // Default version
                    model.filePath = entry.path().string();
                    
                    // Verify file is readable and has content
                    if (!fs::is_regular_file(entry.path())) {
                        std::cerr << ERROR_INVALID_MODEL << entry.path() << " (not a regular file)" << std::endl;
                        continue;
                    }
                    
                    model.sizeBytes = fs::file_size(entry.path());
                    if (model.sizeBytes == 0) {
                        std::cerr << ERROR_INVALID_MODEL << entry.path() << " (empty file)" << std::endl;
                        continue;
                    }
                    
                    model.type = ModelType::GGML;
                    models_.push_back(model);
                } catch (const fs::filesystem_error& e) {
                    std::cerr << ERROR_INVALID_MODEL << entry.path() << " (" << e.what() << ")" << std::endl;
                }
            }
        }
    } catch (const fs::filesystem_error& e) {
        std::cerr << ERROR_SCAN_DIR << e.what() << std::endl;
        throw;
    }
}

std::vector<ModelInfo> ModelManager::getAvailableModels() const {
    return models_;
}

ModelInfo ModelManager::getActiveModel() const {
    return activeModel_;
}

bool ModelManager::loadModel(const std::string& modelId) {
    if (modelId.empty()) {
        std::cerr << "Invalid model ID: empty string" << std::endl;
        return false;
    }
    
    // Find the model
    auto it = std::find_if(models_.begin(), models_.end(),
        [&modelId](const ModelInfo& model) { return model.id == modelId; });
    
    if (it == models_.end()) {
        std::cerr << ERROR_MODEL_NOT_FOUND << modelId << std::endl;
        return false;
    }
    
    // Check if model file exists
    if (!fs::exists(it->filePath)) {
        std::cerr << ERROR_FILE_NOT_FOUND << it->filePath << std::endl;
        return false;
    }
    
    // Verify file is readable and has content
    if (!fs::is_regular_file(it->filePath)) {
        std::cerr << ERROR_INVALID_MODEL << it->filePath << " (not a regular file)" << std::endl;
        return false;
    }
    
    if (fs::file_size(it->filePath) == 0) {
        std::cerr << ERROR_INVALID_MODEL << it->filePath << " (empty file)" << std::endl;
        return false;
    }
    
    // Unload current model if any
    unloadCurrentModel();
    
    // Set new active model
    activeModel_ = *it;
    modelLoaded_ = true;
    
    return true;
}

void ModelManager::unloadCurrentModel() {
    if (modelLoaded_) {
        try {
            // Clean up any resources
            modelLoaded_ = false;
            activeModel_ = ModelInfo();
        } catch (const std::exception& e) {
            std::cerr << "Error unloading model: " << e.what() << std::endl;
        }
    }
}

bool ModelManager::isModelLoaded() const {
    return modelLoaded_;
}

std::shared_ptr<ITranslationModel> ModelManager::getTranslationModel() {
    if (!modelLoaded_) {
        std::cerr << "No model is currently loaded" << std::endl;
        return nullptr;
    }
    
    try {
        // Create and initialize the model with the active model info
        auto model = std::make_shared<GGMLModel>(activeModel_);
        if (!model->initialize()) {
            std::cerr << ERROR_MODEL_INIT << activeModel_.id << std::endl;
            return nullptr;
        }
        
        return model;
    } catch (const std::exception& e) {
        std::cerr << "Error creating translation model: " << e.what() << std::endl;
        return nullptr;
    }
}

bool ModelManager::downloadModel(const std::string& modelId, ProgressCallback callback) {
    if (modelId.empty()) {
        std::cerr << "Invalid model ID: empty string" << std::endl;
        return false;
    }
    
    // Find the model
    auto it = std::find_if(models_.begin(), models_.end(),
        [&modelId](const ModelInfo& model) { return model.id == modelId; });
    
    if (it == models_.end()) {
        std::cerr << ERROR_MODEL_NOT_FOUND << modelId << std::endl;
        return false;
    }
    
    // Check if model already exists
    if (fs::exists(it->filePath)) {
        try {
            // Verify existing file
            if (!fs::is_regular_file(it->filePath)) {
                std::cerr << ERROR_INVALID_MODEL << it->filePath << " (not a regular file)" << std::endl;
                return false;
            }
            
            if (fs::file_size(it->filePath) == 0) {
                std::cerr << ERROR_INVALID_MODEL << it->filePath << " (empty file)" << std::endl;
                return false;
            }
            
            callback(100, "Model already downloaded");
            return true;
        } catch (const fs::filesystem_error& e) {
            std::cerr << "Error checking existing model: " << e.what() << std::endl;
            return false;
        }
    }
    
    try {
        // Simulate download progress
        for (int progress = 0; progress <= 100; progress += 10) {
            callback(progress, "Downloading model...");
            std::this_thread::sleep_for(std::chrono::milliseconds(500));
        }
        
        // Create a dummy model file for testing
        std::ofstream file(it->filePath, std::ios::binary);
        if (!file) {
            std::cerr << ERROR_FILE_CREATE << it->filePath << std::endl;
            return false;
        }
        
        // Write some dummy data
        std::vector<char> dummyData(it->sizeBytes, 0);
        file.write(dummyData.data(), dummyData.size());
        file.close();
        
        return true;
    } catch (const std::exception& e) {
        std::cerr << ERROR_DOWNLOAD_FAILED << e.what() << std::endl;
        return false;
    }
}
