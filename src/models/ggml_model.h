#pragma once

#include <memory>
#include <string>
#include <vector>
#include <thread>
#include "../../include/interfaces/i_translation_model.h"
#include "../translation/data_structures.h"

class ThreadPool;
class InferenceEngine;

class GGMLModel : public ITranslationModel {
public:
    GGMLModel(const ModelInfo& modelInfo);
    ~GGMLModel() override;
    
    bool initialize() override;
    bool isInitialized() const override;
    
    TranslationResult translate(const std::string& text, const TranslationOptions& options) override;
    std::future<TranslationResult> translateAsync(const std::string& text, const TranslationOptions& options) override;
    
    ModelInfo getModelInfo() const override;
    InferenceStats getLastInferenceStats() const override;

private:
    ModelInfo modelInfo_;
    bool initialized_ = false;
    InferenceStats lastStats_;
    
    // Inference engine
    std::unique_ptr<InferenceEngine> engine_;
    
    // Thread pool for async operations
    std::shared_ptr<ThreadPool> threadPool_;
    
    // Helper methods
    std::vector<int> tokenize(const std::string& text);
    std::string detokenize(const std::vector<int>& tokens);
    void runInference(const std::vector<int>& inputTokens, std::vector<int>& outputTokens, const TranslationOptions& options);
};
