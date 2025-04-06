/**
 * @file worker.h
 * @brief Header file for the translation worker thread
 */

#pragma once

#include <string>
#include <memory>
#include <queue>
#include <mutex>
#include <condition_variable>
#include <thread>
#include <functional>
#include "interfaces/i_translation_model.h"
#include "translation/data_structures.h"

namespace koebridge {
namespace translation {

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
    std::string text;                ///< Text to translate
    TranslationOptions options;      ///< Translation options
    TranslationCallback callback;    ///< Callback for result
};

/**
 * @class TranslationWorker
 * @brief Worker class for handling translation requests in a separate thread
 * 
 * This class manages translation requests in a background thread, providing
 * asynchronous translation capabilities and progress reporting.
 */
class TranslationWorker {
public:
    /**
     * @brief Constructor for TranslationWorker
     * @param model Shared pointer to the translation model
     */
    explicit TranslationWorker(std::shared_ptr<ITranslationModel> model);
    
    /**
     * @brief Destructor for TranslationWorker
     */
    ~TranslationWorker();

    /**
     * @brief Start the worker thread
     */
    void start();
    
    /**
     * @brief Stop the worker thread
     */
    void stop();

    /**
     * @brief Add a new translation request to the queue
     * @param text Text to translate
     * @param options Translation options
     * @param callback Callback function for results
     */
    void addRequest(const std::string& text, const TranslationOptions& options, TranslationCallback callback);

private:
    /**
     * @brief Process translation requests from the queue
     */
    void processRequests();
    
    std::shared_ptr<ITranslationModel> model_;     ///< Translation model
    std::queue<TranslationRequest> requestQueue_;  ///< Queue of translation requests
    std::mutex queueMutex_;                        ///< Mutex for queue access
    std::condition_variable queueCondition_;       ///< Condition variable for queue
    std::thread workerThread_;                     ///< Worker thread
    bool running_ = false;                         ///< Worker running flag
};

} // namespace translation
} // namespace koebridge