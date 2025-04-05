/**
 * @file i_model_manager.h
 * @brief Interface definition for model management
 */

#pragma once

#include <string>
#include <vector>
#include "../src/translation/data_structures.h"

namespace koebridge {
namespace translation {

// Forward declarations
struct ModelInfo;
using ProgressCallback = std::function<void(int, const std::string&)>;

/**
 * @class IModelManager
 * @brief Abstract interface for managing translation models
 * 
 * This interface defines the contract for model management operations including
 * model discovery, lifecycle management, and model operations.
 */
class IModelManager {
public:
    /**
     * @brief Virtual destructor
     */
    virtual ~IModelManager() = default;
    
    /**
     * @brief Initialize the model manager
     * @return bool True if initialization was successful, false otherwise
     */
    virtual bool initialize() = 0;
    
    /**
     * @brief Load a translation model by path
     * @param modelPath The path to the model to load
     * @return bool True if model was loaded successfully, false otherwise
     */
    virtual bool loadModel(const std::string& modelPath) = 0;
    
    /**
     * @brief Unload the currently loaded model
     * @return bool True if the model was unloaded successfully, false otherwise
     */
    virtual bool unloadModel() = 0;
    
    /**
     * @brief Get list of available translation models
     * @return std::vector<ModelInfo> List of available models
     */
    virtual std::vector<ModelInfo> getAvailableModels() const = 0;
    
    /**
     * @brief Get information about the currently active model
     * @return ModelInfo Information about the active model
     */
    virtual ModelInfo getActiveModel() const = 0;
    
    /**
     * @brief Check if a model is currently loaded
     * @return bool True if a model is loaded and ready for use
     */
    virtual bool isModelLoaded() const = 0;
    
    /**
     * @brief Download a model by ID
     * @param modelId The unique identifier of the model to download
     * @param callback Progress callback function for download status updates
     * @return bool True if download was successful, false otherwise
     */
    virtual bool downloadModel(const std::string& modelId, ProgressCallback callback) = 0;
};

} // namespace translation
} // namespace koebridge
