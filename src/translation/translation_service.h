/**
 * @file translation_service.h
 * @brief Implementation of the translation service
 */

#pragma once

#include <memory>
#include <QObject>
#include <QThread>
#include <QFuture>
#include <QFutureWatcher>
#include "interfaces/i_translation_service.h"
#include "translation/model_manager.h"

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
    explicit TranslationService(std::shared_ptr<ModelManager> modelManager);

    /**
     * @brief Destructor
     */
    ~TranslationService() override;

    // ITranslationService interface implementation
    bool initialize() override;
    void shutdown() override;
    std::string translateText(const std::string& japaneseText) override;
    void translateTextAsync(const std::string& japaneseText, TranslationCallback callback) override;
    void setTranslationOptions(const TranslationOptions& options) override;
    TranslationOptions getTranslationOptions() const override;

signals:
    /**
     * @brief Signal emitted when translation is complete
     * @param result The translation result
     */
    void translationComplete(const TranslationResult& result);

    /**
     * @brief Signal emitted when translation fails
     * @param errorMessage The error message
     */
    void translationError(const std::string& errorMessage);

private slots:
    /**
     * @brief Slot to handle async translation completion
     * @param result The translation result
     */
    void handleAsyncTranslationComplete(const TranslationResult& result);

private:
    /**
     * @brief Internal translation function
     * @param japaneseText The Japanese text to translate
     * @return TranslationResult The translation result
     */
    TranslationResult translateInternal(const std::string& japaneseText);

    std::shared_ptr<ModelManager> modelManager_;  ///< Model manager instance
    TranslationOptions options_;                  ///< Current translation options
    QThread workerThread_;                        ///< Thread for async operations
    bool initialized_;                            ///< Initialization state
}; 