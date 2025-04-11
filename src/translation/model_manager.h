/**
 * @file model_manager.h
 * @brief Header file for translation model management
 */

#pragma once

#include <string>
#include <vector>
#include <memory>
#include <functional>
#include <mutex>
#include <condition_variable>
#include <queue>
#include <thread>
#include "interfaces/i_model_manager.h"
#include "interfaces/i_translation_model.h"
#include "data_structures.h"

namespace koebridge {
namespace translation {

// Forward declaration
class ITranslationModel;

/**
 * @class ModelManager
 * @brief Manages translation models and their lifecycle
 */
class ModelManager : public IModelManager {
public:
    /**
     * @brief Constructor
     * @param modelPath Optional path to override the config setting
     */
    explicit ModelManager(const std::string& modelPath = "");

    /**
     * @brief Destructor
     */
    ~ModelManager() override;

    // IModelManager interface implementation
    bool initialize() override;
    bool loadModel(const std::string& modelId) override;
    bool unloadModel() override;
    std::vector<ModelInfo> getAvailableModels() const override;
    ModelInfo getActiveModel() const override;
    bool isModelLoaded() const override;
    bool downloadModel(const std::string& modelId, ProgressCallback callback) override;
    std::shared_ptr<ITranslationModel> getTranslationModel() override;

    // Additional methods specific to ModelManager
    /**
     * @brief Translate text synchronously
     * @param text Text to translate
     * @param options Translation options
     * @return TranslationResult Translation result
     */
    TranslationResult translate(const std::string& text, const TranslationOptions& options);

    /**
     * @brief Translate text asynchronously
     * @param text Text to translate
     * @param options Translation options
     * @param progressCallback Optional callback for progress updates
     * @return std::future<TranslationResult> Future containing the translation result
     */
    std::future<TranslationResult> translateAsync(
        const std::string& text,
        const TranslationOptions& options,
        ProgressCallback progressCallback = nullptr
    );

private:
    /**
     * @brief Get the model path from config or use default
     * @return The configured model path
     */
    std::string getModelPathFromConfig() const;

    /**
     * @brief Scan the model directory for available models
     * @return bool True if scan was successful
     */
    bool scanForModels();

    /**
     * @brief Unload the current model (internal implementation)
     */
    void unloadCurrentModel();

    /**
     * @brief Process the translation queue
     */
    void processTranslationQueue();

    /**
     * @brief Add a translation request to the queue
     * @param text Text to translate
     * @param options Translation options
     * @param progressCallback Progress callback
     * @return std::future<TranslationResult> Future for the translation result
     */
    std::future<TranslationResult> queueTranslation(
        const std::string& text,
        const TranslationOptions& options,
        ProgressCallback progressCallback
    );

    std::string modelPath_;                    ///< Path to the model directory
    bool initialized_ = false;                 ///< Initialization state
    bool modelLoaded_ = false;                 ///< Model loading state
    std::vector<ModelInfo> availableModels_;   ///< List of available models
    ModelInfo activeModel_;                    ///< Currently active model
    std::shared_ptr<ITranslationModel> model_; ///< Currently loaded model
    mutable std::mutex modelMutex_;            ///< Mutex for model operations
    mutable std::mutex modelsMutex_;           ///< Mutex for available models list
    mutable std::mutex queueMutex_;            ///< Mutex for translation queue
    std::condition_variable queueCV_;          ///< Condition variable for queue processing
    std::queue<std::function<void()>> translationQueue_; ///< Queue of translation tasks
    std::thread workerThread_;                 ///< Worker thread for processing queue
    bool shouldStop_ = false;                  ///< Flag to signal worker thread to stop
};

} // namespace translation
} // namespace koebridge
