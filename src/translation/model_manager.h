/**
 * @file model_manager.h
 * @brief Header file for translation model management
 */

#pragma once

#include <string>
#include <vector>
#include <memory>
#include "../models/ggml_model.h"
#include "data_structures.h"

/**
 * @class ModelManager
 * @brief Manages translation models and their lifecycle
 */
class ModelManager {
public:
    /**
     * @brief Constructor
     * @param modelPath Path to the directory containing model files
     */
    explicit ModelManager(const std::string& modelPath = "./models");

    /**
     * @brief Get list of available models
     * @return Vector of ModelInfo for available models
     */
    std::vector<ModelInfo> getAvailableModels() const;

    /**
     * @brief Load a model by ID
     * @param modelId ID of the model to load
     * @return bool True if model was loaded successfully
     */
    bool loadModel(const std::string& modelId);

    /**
     * @brief Check if a model is currently loaded
     * @return bool True if a model is loaded
     */
    bool isModelLoaded() const;

    /**
     * @brief Get the currently active model
     * @return ModelInfo Information about the active model
     */
    ModelInfo getActiveModel() const;

private:
    std::string modelPath_;           ///< Path to model directory
    std::vector<ModelInfo> models_;   ///< List of available models
    ModelInfo activeModel_;           ///< Currently active model
    bool modelLoaded_;                ///< Flag indicating if a model is loaded
};
