/**
 * @file model_manager.h
 * @brief Header file for translation model management
 */

#pragma once

#include <string>
#include <vector>
#include <memory>
#include <functional>
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

    // Additional methods specific to ModelManager
    std::shared_ptr<ITranslationModel> getTranslationModel() override;

private:
    /**
     * @brief Get the model path from config or use default
     * @return The configured model path
     */
    std::string getModelPathFromConfig() const;

    /**
     * @brief Scan the model directory for available models
     */
    void scanForModels();

    /**
     * @brief Unload the current model (internal implementation)
     */
    void unloadCurrentModel();

    std::string modelPath_;           ///< Path to model directory
    std::vector<ModelInfo> models_;   ///< List of available models
    ModelInfo activeModel_;           ///< Currently active model
    bool modelLoaded_;                ///< Flag indicating if a model is loaded
    std::shared_ptr<ITranslationModel> translationModel_; ///< The currently loaded translation model
};

} // namespace translation
} // namespace koebridge
