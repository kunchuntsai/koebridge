#include "models/ggml_model.h"
#include "utils/config.h"
#include "utils/logger.h"
#include <iostream>
#include <future>
#include <chrono>

namespace koebridge {
namespace models {

GGMLModel::GGMLModel(const translation::ModelInfo& modelInfo)
    : modelInfo_(modelInfo),
      engine_(std::make_unique<inference::InferenceEngine>()),
      initialized_(false) {
    LOG_INFO("Creating GGML model for: " + modelInfo.id);
}

GGMLModel::~GGMLModel() {
    // Cleanup will be handled by smart pointers
}

bool GGMLModel::initialize() {
    if (initialized_) {
        return true;
    }
    
    LOG_INFO("Initializing GGML model: " + modelInfo_.id);
    
    // Initialize the inference engine
    if (!engine_->initialize(modelInfo_.path)) {
        LOG_ERROR("Failed to initialize inference engine for model: " + modelInfo_.id);
        return false;
    }
    
    initialized_ = true;
    LOG_INFO("GGML model initialized successfully: " + modelInfo_.id);
    return true;
}

bool GGMLModel::isInitialized() const {
    return initialized_;
}

translation::TranslationResult GGMLModel::translate(const std::string& text, const translation::TranslationOptions& options) {
    translation::TranslationResult result;
    result.sourceText = text;
    
    if (!initialized_) {
        result.success = false;
        result.errorMessage = "Model not initialized";
        return result;
    }
    
    try {
        auto startTime = std::chrono::high_resolution_clock::now();
        
        // Process input through the inference engine
        std::string output;
        if (!engine_->processInput(text, output)) {
            result.success = false;
            result.errorMessage = "Translation failed";
            return result;
        }
        
        auto endTime = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);
        
        result.text = output;
        result.success = true;
        result.metrics.totalTimeMs = duration.count();
        
        // Get inference stats
        translation::InferenceStats stats;
        engine_->runInference(std::vector<int>(), options, stats);
        result.metrics.inferenceTimeMs = stats.inferenceTimeMs;
        result.metrics.inputTokenCount = stats.inputTokenCount;
        
    } catch (const std::exception& e) {
        result.success = false;
        result.errorMessage = std::string("Translation error: ") + e.what();
    }
    
    return result;
}

std::future<translation::TranslationResult> GGMLModel::translateAsync(
    const std::string& text,
    const translation::TranslationOptions& options
) {
    return std::async(std::launch::async, [this, text, options]() {
        return translate(text, options);
    });
}

translation::ModelInfo GGMLModel::getModelInfo() const {
    return modelInfo_;
}

translation::InferenceStats GGMLModel::getLastInferenceStats() const {
    translation::InferenceStats stats;
    engine_->runInference(std::vector<int>(), translation::TranslationOptions(), stats);
    return stats;
}

} // namespace models
} // namespace koebridge
