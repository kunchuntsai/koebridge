#include "worker.h"

TranslationWorker::TranslationWorker(std::shared_ptr<ITranslationModel> model)
    : model_(model), running_(true) {
}

TranslationWorker::~TranslationWorker() {
    stop();
}

void TranslationWorker::addRequest(const std::string& text, const TranslationOptions& options, TranslationCallback callback) {
    QMutexLocker locker(&mutex_);
    TranslationRequest request;
    request.text = text;
    request.options = options;
    request.callback = callback;
    requestQueue_.enqueue(request);
    condition_.wakeOne();
}

void TranslationWorker::stop() {
    QMutexLocker locker(&mutex_);
    running_ = false;
    condition_.wakeAll();
}

void TranslationWorker::process() {
    while (true) {
        TranslationRequest request;
        
        {
            QMutexLocker locker(&mutex_);
            
            // Wait for a request if the queue is empty
            while (running_ && requestQueue_.isEmpty()) {
                condition_.wait(&mutex_);
            }
            
            // Check if worker should stop
            if (!running_ && requestQueue_.isEmpty()) {
                break;
            }
            
            // Get next request from queue
            request = requestQueue_.dequeue();
        }
        
        // Process the translation request
        try {
            // Perform the translation
            TranslationResult result = model_->translate(request.text, request.options);
            
            // Call the callback with the result
            if (request.callback) {
                request.callback(result);
            }
            
            // Emit progress signal (100% complete)
            emit translationProgress(100);
        } catch (const std::exception& e) {
            // Handle any exceptions during translation
            TranslationResult errorResult;
            errorResult.sourceText = request.text;
            errorResult.success = false;
            errorResult.errorMessage = std::string("Translation error: ") + e.what();
            
            // Call the callback with the error result
            if (request.callback) {
                request.callback(errorResult);
            }
        }
    }
    
    emit finished();
}