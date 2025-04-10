/**
 * @file translation_service.h
 * @brief Implementation of the translation service
 */

#pragma once

#include <memory>
#include <string>
#include <functional>
#include <QObject>
#include <QString>
#include "data_structures.h"
#include "interfaces/i_translation_service.h"
#include "interfaces/i_model_manager.h"

namespace koebridge {
namespace translation {

// Define the callback type for async translation
using TranslationCallback = std::function<void(const TranslationResult&)>;

/**
 * @class TranslationService
 * @brief Implementation of the translation service interface
 *
 * This class provides the concrete implementation of the translation service,
 * handling both synchronous and asynchronous translation operations.
 */
class TranslationService : public QObject, public ITranslationService {
    Q_OBJECT

public:
    /**
     * @brief Constructor
     * @param modelManager Shared pointer to the model manager
     */
    explicit TranslationService(std::shared_ptr<IModelManager> modelManager);

    /**
     * @brief Destructor
     */
    ~TranslationService() override;

    // ITranslationService interface implementation
    bool initialize() override;
    bool translate(const std::string& input, std::string& output) override;
    void setOptions(const TranslationOptions& options) override;
    TranslationOptions getOptions() const override;
    bool isInitialized() const override;

    // Additional methods not in the interface
    void shutdown();
    std::string translateText(const std::string& japaneseText);
    void translateTextAsync(const std::string& japaneseText, TranslationCallback callback);
    void setTranslationOptions(const TranslationOptions& options);
    TranslationOptions getTranslationOptions() const;

signals:
    /**
     * @brief Signal emitted when translation is complete
     * @param input The input text
     * @param output The translated text
     */
    void translationComplete(const QString& input, const QString& output);

    /**
     * @brief Signal emitted when translation fails
     * @param message The error message
     */
    void error(const QString& message);

    /**
     * @brief Signal emitted when progress is updated
     * @param progress The progress percentage
     */
    void progressUpdated(int progress);

private:
    /**
     * @brief Internal translation function
     * @param japaneseText The Japanese text to translate
     * @return TranslationResult The translation result
     */
    TranslationResult translateInternal(const std::string& japaneseText);

    bool validateInput(const std::string& input) const;
    void handleTranslationError(const std::string& message);
    void updateProgress(int progress);

    std::shared_ptr<IModelManager> modelManager_;  ///< Model manager instance
    TranslationOptions options_;                  ///< Current translation options
    bool initialized_;                            ///< Initialization state
};

} // namespace translation
} // namespace koebridge