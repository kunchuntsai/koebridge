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
    , modelLoaded_(false)
    , shouldStop_(false) {
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
    // Start worker thread for async translations
    workerThread_ = std::thread(&ModelManager::processTranslationQueue, this);

    // Scan for available models
    scanForModels();
    return true;
}

std::vector<ModelInfo> ModelManager::getAvailableModels() const {
    std::lock_guard<std::mutex> lock(modelsMutex_);
    return models_;
}

ModelInfo ModelManager::getActiveModel() const {
    std::lock_guard<std::mutex> lock(modelMutex_);
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
    for (const auto& model : models_) {
        if (model.id == modelId) {
            // Unload current model if any
            if (modelLoaded_) {
                unloadModel();
            }

            // Create and initialize the GGML model
            translationModel_ = std::make_shared<models::GGMLModel>(model);
            if (!translationModel_->initialize()) {
                LOG_ERROR("Failed to initialize model: " + modelId);
                translationModel_.reset();
                return false;
            }

            // Update model info
            activeModel_ = model;
            modelLoaded_ = true;
            LOG_INFO("Successfully loaded model: " + modelId);
            return true;
        }
    }

    LOG_ERROR("Model not found: " + modelId);
    return false;
}

bool ModelManager::unloadModel() {
    std::lock_guard<std::mutex> modelLock(modelMutex_);
    if (!modelLoaded_) {
        return true;  // Nothing to unload
    }

    unloadCurrentModel();
    return true;
}

bool ModelManager::isModelLoaded() const {
    std::lock_guard<std::mutex> lock(modelMutex_);
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
    std::lock_guard<std::mutex> lock(modelMutex_);
    if (!modelLoaded_) {
        return nullptr;
    }

    return translationModel_;
}

TranslationResult ModelManager::translate(const std::string& text, const TranslationOptions& options) {
    std::lock_guard<std::mutex> lock(modelMutex_);
    if (!modelLoaded_ || !translationModel_) {
        TranslationResult result;
        result.success = false;
        result.errorMessage = "No model loaded";
        return result;
    }

    return translationModel_->translate(text, options);
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
    std::lock_guard<std::mutex> lock(modelsMutex_);
    models_.clear();

    QDir modelDir(QString::fromStdString(modelPath_));
    if (!modelDir.exists()) {
        LOG_ERROR("Model directory does not exist: " + modelPath_);
        return;
    }

    QFileInfoList files = modelDir.entryInfoList(QDir::Files | QDir::NoDotAndDotDot);
    for (const QFileInfo& file : files) {
        if (file.suffix() == "bin" || file.suffix() == "ggml") {
            ModelInfo info;
            info.id = file.baseName().toStdString();
            info.path = file.absoluteFilePath().toStdString();
            info.size = file.size();
            info.lastModified = file.lastModified().toSecsSinceEpoch();

            // Detect model type and capabilities
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
            LOG_INFO("Added model: " + info.id + " at " + info.path + " (Type: " + info.modelType + ")");
        }
    }

    LOG_INFO("Total models found: " + std::to_string(models_.size()));
}

} // namespace translation
} // namespace koebridge
