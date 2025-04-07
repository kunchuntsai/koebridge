/**
 * @file llm_manager.h
 * @brief Header file for the LLM model manager
 */

#pragma once

#include <string>
#include <memory>
#include <vector>
#include <map>
#include "llm/llm_model.h"
#include "translation/model_manager.h"

namespace koebridge {
namespace llm {

/**
 * @class LLMManager
 * @brief Manager for LLM models and operations
 * 
 * This class manages the lifecycle of LLM models, providing a high-level interface
 * for model loading, configuration, and text generation operations.
 */
class LLMManager {
public:
    /**
     * @brief Constructor
     * @param modelManager Reference to the translation model manager
     */
    explicit LLMManager(std::shared_ptr<translation::ModelManager> modelManager);
    
    /**
     * @brief Destructor
     */
    ~LLMManager();
    
    /**
     * @brief Initialize the LLM manager
     * @return bool True if initialization was successful
     */
    bool initialize();
    
    /**
     * @brief Load an LLM model by ID
     * @param modelId ID of the model to load
     * @param config Configuration options for the model
     * @return bool True if model was loaded successfully
     */
    bool loadModel(const std::string& modelId, const LLMConfig& config = LLMConfig());
    
    /**
     * @brief Unload the currently loaded model
     * @return bool True if unloaded successfully
     */
    bool unloadModel();
    
    /**
     * @brief Check if a model is loaded
     * @return bool True if a model is loaded
     */
    bool isModelLoaded() const;
    
    /**
     * @brief Get information about the active model
     * @return ModelInfo Information about the active model
     */
    translation::ModelInfo getActiveModel() const;
    
    /**
     * @brief Get the loaded LLM model
     * @return std::shared_ptr<LLMModel> Shared pointer to the model
     */
    std::shared_ptr<LLMModel> getModel();
    
    /**
     * @brief Generate text completion from a prompt
     * @param prompt The input prompt
     * @return LLMOutput The generated completion
     */
    LLMOutput complete(const std::string& prompt);
    
    /**
     * @brief Generate text completion asynchronously
     * @param prompt The input prompt
     * @return std::future<LLMOutput> Future containing the generated completion
     */
    std::future<LLMOutput> completeAsync(const std::string& prompt);
    
    /**
     * @brief Set configuration for the active model
     * @param config New configuration
     */
    void setConfig(const LLMConfig& config);
    
    /**
     * @brief Get configuration of the active model
     * @return LLMConfig Current configuration
     */
    LLMConfig getConfig() const;
    
private:
    std::shared_ptr<translation::ModelManager> modelManager_; ///< Translation model manager
    std::shared_ptr<LLMModel> model_;                        ///< Currently loaded LLM model
    bool initialized_;                                       ///< Initialization state
    translation::ModelInfo activeModel_;                     ///< Currently active model info
    LLMConfig config_;                                       ///< Current configuration
    
    /**
     * @brief Create an LLM model instance for the given model info
     * @param modelInfo Information about the model
     * @param config Configuration options
     * @return std::shared_ptr<LLMModel> Shared pointer to the created model
     */
    std::shared_ptr<LLMModel> createModel(const translation::ModelInfo& modelInfo, const LLMConfig& config);
};

} // namespace llm
} // namespace koebridge 