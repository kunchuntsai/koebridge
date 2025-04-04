/**
 * @file worker.h
 * @brief Header file for the translation worker thread
 */

#pragma once

#include <QObject>
#include <QQueue>
#include <QMutex>
#include <QWaitCondition>
#include <memory>
#include <functional>
#include "data_structures.h"
#include "../include/interfaces/i_translation_model.h"

/**
 * @typedef TranslationCallback
 * @brief Callback function type for translation results
 */
typedef std::function<void(const TranslationResult&)> TranslationCallback;

/**
 * @struct TranslationRequest
 * @brief Structure representing a translation request
 */
struct TranslationRequest {
    std::string text;                    ///< Text to translate
    TranslationOptions options;          ///< Translation options
    TranslationCallback callback;        ///< Callback function for results
};

/**
 * @class TranslationWorker
 * @brief Worker class for handling translation requests in a separate thread
 * 
 * This class manages translation requests in a background thread, providing
 * asynchronous translation capabilities and progress reporting.
 */
class TranslationWorker : public QObject {
    Q_OBJECT

public:
    /**
     * @brief Constructor for TranslationWorker
     * @param model Shared pointer to the translation model
     */
    explicit TranslationWorker(std::shared_ptr<ITranslationModel> model);
    
    /**
     * @brief Destructor for TranslationWorker
     */
    ~TranslationWorker() override;

    /**
     * @brief Add a new translation request to the queue
     * @param text Text to translate
     * @param options Translation options
     * @param callback Callback function for results
     */
    void addRequest(const std::string& text, const TranslationOptions& options, TranslationCallback callback);
    
    /**
     * @brief Stop the worker thread
     */
    void stop();

signals:
    /**
     * @brief Signal emitted when all requests are processed
     */
    void finished();
    
    /**
     * @brief Signal emitted to report translation progress
     * @param percentComplete Progress percentage (0-100)
     */
    void translationProgress(int percentComplete);

public slots:
    /**
     * @brief Process translation requests from the queue
     */
    void process();

private:
    std::shared_ptr<ITranslationModel> model_;     ///< Translation model
    QQueue<TranslationRequest> requestQueue_;       ///< Queue of pending requests
    QMutex mutex_;                                 ///< Mutex for thread synchronization
    QWaitCondition condition_;                     ///< Condition variable for thread coordination
    bool running_;                                 ///< Worker thread running flag
};