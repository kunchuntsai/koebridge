/**
 * @file translation_service.cc
 * @brief Implementation of the translation service
 */

#include "translation_service.h"
#include <QFuture>
#include <QtConcurrent>

TranslationService::TranslationService(std::shared_ptr<ModelManager> modelManager)
    : modelManager_(std::move(modelManager))
    , initialized_(false) {
    // Initialize with default options
    options_.temperature = 0.7f;
    options_.maxLength = 1024;
    options_.beamSize = 4;
    options_.style = TranslationOptions::Style::NATURAL;
    options_.timeoutMs = 30000;
}

TranslationService::~TranslationService() {
    shutdown();
}

bool TranslationService::initialize() {
    if (initialized_) {
        return true;
    }

    // Initialize model manager
    if (!modelManager_->loadModel("default_model")) {
        emit translationError("Failed to load default model");
        return false;
    }

    initialized_ = true;
    return true;
}

void TranslationService::shutdown() {
    if (!initialized_) {
        return;
    }

    // Clean up resources
    workerThread_.quit();
    workerThread_.wait();

    initialized_ = false;
}

std::string TranslationService::translateText(const std::string& japaneseText) {
    if (!initialized_) {
        emit translationError("Translation service not initialized");
        return "";
    }

    auto result = translateInternal(japaneseText);
    if (!result.success) {
        emit translationError(result.errorMessage);
        return "";
    }

    emit translationComplete(result);
    return result.translatedText;
}

void TranslationService::translateTextAsync(const std::string& japaneseText, TranslationCallback callback) {
    if (!initialized_) {
        emit translationError("Translation service not initialized");
        callback(TranslationResult{});
        return;
    }

    // Create a future for async translation
    QFuture<TranslationResult> future = QtConcurrent::run([this, japaneseText]() {
        return translateInternal(japaneseText);
    });

    // Create a watcher to handle the completion
    auto* watcher = new QFutureWatcher<TranslationResult>(this);
    watcher->setFuture(future);

    // Connect the watcher's finished signal to handle completion
    connect(watcher, &QFutureWatcher<TranslationResult>::finished, this, [this, watcher, callback]() {
        auto result = watcher->result();
        if (!result.success) {
            emit translationError(result.errorMessage);
        } else {
            emit translationComplete(result);
        }
        callback(result);
        watcher->deleteLater();
    });
}

void TranslationService::setTranslationOptions(const TranslationOptions& options) {
    options_ = options;
}

TranslationOptions TranslationService::getTranslationOptions() const {
    return options_;
}

TranslationResult TranslationService::translateInternal(const std::string& japaneseText) {
    TranslationResult result;
    result.sourceText = japaneseText;
    
    try {
        // Get the active model
        auto modelInfo = modelManager_->getActiveModel();
        if (modelInfo.id.empty()) {
            result.success = false;
            result.errorMessage = "No active model";
            return result;
        }

        // TODO: Implement actual translation using the model
        // For now, return a placeholder result
        result.translatedText = "Placeholder translation";
        result.success = true;
        result.metrics.totalTimeMs = 0.0;
        result.metrics.inferenceTimeMs = 0.0;
        result.metrics.preprocessTimeMs = 0.0;
        result.metrics.postprocessTimeMs = 0.0;
        result.metrics.inputTokenCount = 0;
        result.metrics.outputTokenCount = 0;

    } catch (const std::exception& e) {
        result.success = false;
        result.errorMessage = e.what();
    }

    return result;
}

void TranslationService::handleAsyncTranslationComplete(const TranslationResult& result) {
    if (!result.success) {
        emit translationError(result.errorMessage);
    }
} 