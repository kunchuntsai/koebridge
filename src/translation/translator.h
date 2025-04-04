/**
 * @file translator.h
 * @brief Header file for the main translation component
 */

#pragma once

#include <string>
#include "model_manager.h"

/**
 * @class Translator
 * @brief Main translation component that coordinates translation operations
 * 
 * This class serves as the main entry point for translation operations,
 * coordinating between the model manager and other components to provide
 * translation functionality.
 */
class Translator {
public:
    /**
     * @brief Constructor for Translator
     */
    Translator();
    
    /**
     * @brief Initialize the translator with a model
     * @param modelPath Path to the translation model file
     * @return bool True if initialization was successful, false otherwise
     */
    bool initialize(const std::string& modelPath);
    
    /**
     * @brief Translate source text to target language
     * @param sourceText The text to translate
     * @return std::string The translated text
     */
    std::string translate(const std::string& sourceText);
    
private:
    ModelManager modelManager; ///< Manager for handling translation models
};
