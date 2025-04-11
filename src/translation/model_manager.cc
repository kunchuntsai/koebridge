/**
 * @file model_manager.cc
 * @brief Implementation of the translation model manager
 */

#include "translation/model_manager.h"
#include "utils/logger.h"
#include "models/ggml_model.h"
#include <QDir>
#include <QFileInfo>
#include <QStandardPaths>
#include <iostream>
#include <fstream>

namespace koebridge {
namespace translation {

ModelManager::ModelManager(const std::string& modelPath)
    : modelPath_(modelPath)
    , modelLoaded_(false)
    , shouldStop_(false) {
    // Model path is provided by the caller
}

ModelManager::~ModelManager() {
    // Signal worker thread to stop
    {
        std::lock_guard<std::mutex> lock(queueMutex_);
        shouldStop_ = true;
    }
    queueCV_.notify_one();

    // Wait for worker thread to finish
    if (workerThread_.joinable()) {
        workerThread_.join();
    }

    unloadCurrentModel();
}

bool ModelManager::initialize() {
    if (initialized_) {
        return true;
    }

    if (modelPath_.empty()) {
        utils::Logger::getInstance().error("Model path is empty");
        return false;
    }

    utils::Logger::getInstance().info("Initializing model manager with path: " + modelPath_);

    // Scan for available models
    if (!scanForModels()) {
        utils::Logger::getInstance().error("Failed to scan for models");
        return false;
    }

    // Try to load a model
    if (!availableModels_.empty()) {
        utils::Logger::getInstance().info("Attempting to load a model: " + availableModels_[0].id);
        if (!loadModel(availableModels_[0].id)) {
            utils::Logger::getInstance().warning("Failed to load a model: " + availableModels_[0].id);
            return false;
        }
        utils::Logger::getInstance().info("Successfully loaded a model: " + availableModels_[0].id);
    } else {
        utils::Logger::getInstance().warning("No models found in path: " + modelPath_);
        return false;
    }

    initialized_ = true;
    return true;
}

std::vector<ModelInfo> ModelManager::getAvailableModels() const {
    std::lock_guard<std::mutex> lock(queueMutex_);
    return availableModels_;
}

ModelInfo ModelManager::getActiveModel() const {
    std::lock_guard<std::mutex> lock(queueMutex_);
    if (!modelLoaded_) {
        return ModelInfo{};
    }
    return activeModel_;
}

bool ModelManager::loadModel(const std::string& modelId) {
    std::lock_guard<std::mutex> modelsLock(modelsMutex_);
    std::lock_guard<std::mutex> modelLock(modelMutex_);

    // Check if this model is already loaded
    if (modelLoaded_ && activeModel_.id == modelId) {
        return true;
    }

    // Find the model in the available models list
    for (const auto& model : availableModels_) {
        if (model.id == modelId) {
            // Unload current model if any
            if (modelLoaded_) {
                unloadModel();
            }

            utils::Logger::getInstance().info("Creating GGML model for: " + modelId);

            // Create and initialize the GGML model
            model_ = std::make_shared<models::GGMLModel>(model);

            utils::Logger::getInstance().info("Starting model initialization for: " + modelId);
            if (!model_->initialize()) {
                utils::Logger::getInstance().error("Failed to initialize model: " + modelId);
                model_.reset();
                return false;
            }

            // Update model info
            activeModel_ = model;
            modelLoaded_ = true;
            utils::Logger::getInstance().info("Successfully loaded model: " + modelId);
            return true;
        }
    }

    utils::Logger::getInstance().error("Model not found: " + modelId);
    return false;
}

bool ModelManager::unloadModel() {
    std::lock_guard<std::mutex> lock(queueMutex_);
    if (!modelLoaded_) {
        return true;  // Nothing to unload
    }

    unloadCurrentModel();
    return true;
}

bool ModelManager::isModelLoaded() const {
    std::lock_guard<std::mutex> lock(queueMutex_);
    return modelLoaded_;
}

bool ModelManager::downloadModel(const std::string& modelId, ProgressCallback callback) {
    QDir modelDir(QString::fromStdString(modelPath_));
    if (!modelDir.exists()) {
        if (callback) {
            callback(0, "Model directory does not exist: " + modelPath_);
        }
        utils::Logger::getInstance().error("Model directory does not exist: " + modelPath_);
        return false;
    }

    QString modelPath = modelDir.filePath(QString::fromStdString(modelId + ".bin"));
    QFileInfo modelFile(modelPath);

    if (modelFile.exists()) {
        if (callback) {
            callback(100, "Model already exists: " + modelId);
        }
        utils::Logger::getInstance().info("Model already exists: " + modelId);
        return true;
    }

    if (callback) {
        callback(0, "Model not found. Please use download_model.sh script to download: " + modelId);
    }
    utils::Logger::getInstance().info("Model not found. Please use download_model.sh script to download: " + modelId);
    return false;
}

void ModelManager::unloadCurrentModel() {
    // Reset the state
    model_.reset();
    modelLoaded_ = false;
    activeModel_ = ModelInfo{};
}

std::shared_ptr<ITranslationModel> ModelManager::getTranslationModel() {
    std::lock_guard<std::mutex> lock(queueMutex_);
    if (!modelLoaded_) {
        return nullptr;
    }

    return model_;
}

TranslationResult ModelManager::translate(const std::string& text, const TranslationOptions& options) {
    std::lock_guard<std::mutex> lock(queueMutex_);
    if (!modelLoaded_ || !model_) {
        TranslationResult result;
        result.success = false;
        result.errorMessage = "No model loaded";
        return result;
    }

    return model_->translate(text, options);
}

std::future<TranslationResult> ModelManager::translateAsync(
    const std::string& text,
    const TranslationOptions& options,
    ProgressCallback progressCallback
) {
    return queueTranslation(text, options, progressCallback);
}

void ModelManager::processTranslationQueue() {
    while (true) {
        std::function<void()> task;

        {
            std::unique_lock<std::mutex> lock(queueMutex_);
            queueCV_.wait(lock, [this] {
                return !translationQueue_.empty() || shouldStop_;
            });

            if (shouldStop_ && translationQueue_.empty()) {
                break;
            }

            if (!translationQueue_.empty()) {
                task = std::move(translationQueue_.front());
                translationQueue_.pop();
            }
        }

        if (task) {
            task();
        }
    }
}

std::future<TranslationResult> ModelManager::queueTranslation(
    const std::string& text,
    const TranslationOptions& options,
    ProgressCallback progressCallback
) {
    auto promise = std::make_shared<std::promise<TranslationResult>>();
    auto future = promise->get_future();

    auto task = [this, text, options, progressCallback, promise]() {
        try {
            if (progressCallback) {
                progressCallback(0, "Starting translation");
            }

            auto result = translate(text, options);

            if (progressCallback) {
                progressCallback(100, "Translation completed");
            }

            promise->set_value(std::move(result));
        } catch (const std::exception& e) {
            TranslationResult errorResult;
            errorResult.success = false;
            errorResult.errorMessage = std::string("Translation error: ") + e.what();
            promise->set_value(std::move(errorResult));
        }
    };

    {
        std::lock_guard<std::mutex> lock(queueMutex_);
        translationQueue_.push(std::move(task));
    }
    queueCV_.notify_one();

    return future;
}

bool ModelManager::scanForModels() {
    try {
        QDir dir(QString::fromStdString(modelPath_));
        if (!dir.exists()) {
            utils::Logger::getInstance().error("Model directory does not exist: " + modelPath_);
            return false;
        }

        // Clear existing models
        availableModels_.clear();

        // Get all files in the directory
        QFileInfoList files = dir.entryInfoList(QDir::Files | QDir::NoDotAndDotDot);
        for (const QFileInfo& file : files) {
            // Check if file is a model file (has .bin extension)
            if (file.suffix() == "bin") {
                ModelInfo modelInfo;
                modelInfo.id = file.baseName().toStdString();
                modelInfo.path = file.absoluteFilePath().toStdString();
                modelInfo.size = file.size();
                modelInfo.lastModified = file.lastModified().toSecsSinceEpoch();
                availableModels_.push_back(modelInfo);
                utils::Logger::getInstance().info("Found model: " + modelInfo.id);
            }
        }

        return true;
    } catch (const std::exception& e) {
        utils::Logger::getInstance().error("Error scanning for models: " + std::string(e.what()));
        return false;
    }
}

} // namespace translation
} // namespace koebridge
