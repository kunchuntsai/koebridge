/**
 * @file translator.h
 * @brief Header file for the main translation component
 */

#pragma once

#include <memory>
#include <string>
#include <QObject>
#include <QString>
#include "data_structures.h"
#include "model_manager.h"

namespace koebridge {
namespace translation {

/**
 * @class Translator
 * @brief Main translation component that coordinates translation operations
 * 
 * This class serves as the main entry point for translation operations,
 * coordinating between the model manager and other components to provide
 * translation functionality.
 */
class Translator : public QObject {
    Q_OBJECT

public:
    /**
     * @brief Constructor for Translator
     */
    explicit Translator(std::shared_ptr<ModelManager> modelManager);
    
    /**
     * @brief Initialize the translator with a model
     * @param modelPath Path to the translation model file
     * @return bool True if initialization was successful, false otherwise
     */
    bool initialize();
    
    /**
     * @brief Translate source text to target language
     * @param sourceText The text to translate
     * @return std::string The translated text
     */
    bool translate(const std::string& input, std::string& output);
    
    void setOptions(const TranslationOptions& options);
    TranslationOptions getOptions() const;
    bool isInitialized() const;

    ~Translator() override;

signals:
    void translationComplete(const QString& input, const QString& output);
    void error(const QString& message);
    void progressUpdated(int progress);

private:
    bool validateInput(const std::string& input) const;
    void handleTranslationError(const std::string& message);
    void updateProgress(int progress);

    std::shared_ptr<ModelManager> modelManager_;  ///< Manager for handling translation models
    TranslationOptions options_;                  ///< Current translation options
    bool initialized_;                            ///< Initialization status
};

} // namespace translation
} // namespace koebridge
