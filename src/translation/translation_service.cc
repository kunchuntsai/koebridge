/**
 * @file translation_service.cc
 * @brief Implementation of the translation service
 */

#include "translation/translation_service.h"
#include "utils/logger.h"
#include "utils/config.h"
#include <QThread>
#include <QFuture>
#include <QFutureWatcher>
#include <QtConcurrent>

namespace koebridge {
namespace translation {

TranslationService::TranslationService(std::shared_ptr<IModelManager> modelManager)
    : modelManager_(std::move(modelManager))
    , initialized_(false) {
    
    // Initialize default options
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
    
    if (!modelManager_) {
        emit error("Model manager not initialized");
        return false;
    }
    
    initialized_ = true;
    return true;
}

bool TranslationService::isInitialized() const {
    return initialized_;
}

void TranslationService::shutdown() {
    initialized_ = false;
}

bool TranslationService::translate(const std::string& input, std::string& output) {
    if (!initialized_) {
        emit error("Translation service not initialized");
        return false;
    }
    
    if (!validateInput(input)) {
        emit error("Invalid input text");
        return false;
    }
    
    try {
        auto result = translateInternal(input);
        if (result.success) {
            output = result.text;
            emit translationComplete(QString::fromStdString(input), 
                                  QString::fromStdString(output));
            return true;
        } else {
            emit error(QString::fromStdString(result.errorMessage));
            return false;
        }
    } catch (const std::exception& e) {
        handleTranslationError(e.what());
        return false;
    }
}

std::string TranslationService::translateText(const std::string& japaneseText) {
    if (!initialized_) {
        emit error("Translation service not initialized");
        return "";
    }
    
    if (!validateInput(japaneseText)) {
        emit error("Invalid input text");
        return "";
    }
    
    try {
        auto result = translateInternal(japaneseText);
        if (result.success) {
            emit translationComplete(QString::fromStdString(japaneseText), 
                                  QString::fromStdString(result.text));
            return result.text;
        } else {
            emit error(QString::fromStdString(result.errorMessage));
            return "";
        }
    } catch (const std::exception& e) {
        handleTranslationError(e.what());
        return "";
    }
}

void TranslationService::translateTextAsync(const std::string& japaneseText, TranslationCallback callback) {
    if (!initialized_) {
        emit error("Translation service not initialized");
        callback(TranslationResult{});
        return;
    }
    
    if (!validateInput(japaneseText)) {
        emit error("Invalid input text");
        callback(TranslationResult{});
        return;
    }
    
    // Create a future watcher for async operation
    auto* watcher = new QFutureWatcher<TranslationResult>(this);
    
    // Connect signals
    connect(watcher, &QFutureWatcher<TranslationResult>::finished, this, [this, watcher, callback, japaneseText]() {
        try {
            auto result = watcher->result();
            if (result.success) {
                emit translationComplete(QString::fromStdString(japaneseText), 
                                      QString::fromStdString(result.text));
                callback(result);
            } else {
                emit error(QString::fromStdString(result.errorMessage));
                callback(TranslationResult{});
            }
        } catch (const std::exception& e) {
            handleTranslationError(e.what());
            callback(TranslationResult{});
        }
        watcher->deleteLater();
    });
    
    // Start async operation
    QFuture<TranslationResult> future = QtConcurrent::run([this, japaneseText]() {
        return translateInternal(japaneseText);
    });
    
    watcher->setFuture(future);
}

void TranslationService::setOptions(const TranslationOptions& options) {
    options_ = options;
}

void TranslationService::setTranslationOptions(const TranslationOptions& options) {
    setOptions(options);
}

TranslationOptions TranslationService::getOptions() const {
    return options_;
}

TranslationOptions TranslationService::getTranslationOptions() const {
    return getOptions();
}

TranslationResult TranslationService::translateInternal(const std::string& japaneseText) {
    // TODO: Implement actual translation logic
    TranslationResult result;
    result.sourceText = japaneseText;
    result.text = "Translated: " + japaneseText;
    result.success = true;
    return result;
}

bool TranslationService::validateInput(const std::string& input) const {
    return !input.empty();
}

void TranslationService::handleTranslationError(const std::string& message) {
    emit error(QString::fromStdString(message));
}

void TranslationService::updateProgress(int progress) {
    emit progressUpdated(progress);
}

} // namespace translation
} // namespace koebridge 