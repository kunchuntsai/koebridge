#pragma once

#include <QObject>
#include <QQueue>
#include <QMutex>
#include <QWaitCondition>
#include <memory>
#include <functional>
#include "data_structures.h"
#include "../include/interfaces/i_translation_model.h"

// Define TranslationCallback if it's not defined in data_structures.h
// This is a typical definition for a callback function
typedef std::function<void(const TranslationResult&)> TranslationCallback;

struct TranslationRequest {
    std::string text;
    TranslationOptions options;
    TranslationCallback callback;
};

class TranslationWorker : public QObject {
    Q_OBJECT

public:
    explicit TranslationWorker(std::shared_ptr<ITranslationModel> model);
    ~TranslationWorker() override;

    void addRequest(const std::string& text, const TranslationOptions& options, TranslationCallback callback);
    void stop();

signals:
    void finished();
    void translationProgress(int percentComplete);

public slots:
    void process();

private:
    std::shared_ptr<ITranslationModel> model_;
    QQueue<TranslationRequest> requestQueue_;
    QMutex mutex_;
    QWaitCondition condition_;
    bool running_;
};