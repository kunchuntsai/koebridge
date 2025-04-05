#include "translation/worker.h"
#include <iostream>

namespace koebridge {
namespace translation {

TranslationWorker::TranslationWorker(std::shared_ptr<ITranslationModel> model)
    : model_(model), running_(false) {
}

TranslationWorker::~TranslationWorker() {
    stop();
}

void TranslationWorker::start() {
    if (running_) {
        return;
    }
    
    running_ = true;
    workerThread_ = std::thread(&TranslationWorker::processRequests, this);
}

void TranslationWorker::stop() {
    if (!running_) {
        return;
    }
    
    {
        std::lock_guard<std::mutex> lock(queueMutex_);
        running_ = false;
    }
    queueCondition_.notify_all();
    
    if (workerThread_.joinable()) {
        workerThread_.join();
    }
}

void TranslationWorker::addRequest(const std::string& text, const TranslationOptions& options, TranslationCallback callback) {
    TranslationRequest request{text, options, callback};
    
    {
        std::lock_guard<std::mutex> lock(queueMutex_);
        requestQueue_.push(request);
    }
    queueCondition_.notify_one();
}

void TranslationWorker::processRequests() {
    while (true) {
        TranslationRequest request;
        
        {
            std::unique_lock<std::mutex> lock(queueMutex_);
            queueCondition_.wait(lock, [this]() {
                return !running_ || !requestQueue_.empty();
            });
            
            if (!running_ && requestQueue_.empty()) {
                break;
            }
            
            if (!requestQueue_.empty()) {
                request = requestQueue_.front();
                requestQueue_.pop();
            }
        }
        
        if (request.callback) {
            auto result = model_->translate(request.text, request.options);
            request.callback(result);
        }
    }
}

} // namespace translation
} // namespace koebridge