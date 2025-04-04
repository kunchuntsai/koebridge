/**
 * @file model_manager.h
 * @brief Header file for translation model management
 */

#pragma once

#include <string>

/**
 * @class ModelManager
 * @brief Manager class for handling translation models
 * 
 * This class manages the lifecycle of translation models, including
 * loading, unloading, and providing access to the models for translation
 * operations.
 */
class ModelManager {
public:
    /**
     * @brief Constructor for ModelManager
     */
    ModelManager();
    
    /**
     * @brief Destructor for ModelManager
     */
    ~ModelManager();
    
    /**
     * @brief Load a translation model from a file
     * @param modelPath Path to the model file
     * @return bool True if model was loaded successfully, false otherwise
     */
    bool loadModel(const std::string& modelPath);
    
private:
    void* model; ///< Pointer to the loaded translation model
};
