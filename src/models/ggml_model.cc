#include "models/ggml_model.h"
#include "utils/config.h"
#include <iostream>
#include <future>

namespace koebridge {
namespace models {

GGMLModel::GGMLModel(const translation::ModelInfo& modelInfo)
    : modelInfo_(modelInfo),
      engine_(std::make_unique<inference::InferenceEngine>()),
      initialized_(false) {
}

GGMLModel::~GGMLModel() {
    // Cleanup will be handled by smart pointers
}

bool GGMLModel::initialize() {
    if (initialized_) {
        return true;
    }
    
    initialized_ = engine_->initialize(modelInfo_.path);
    
    if (initialized_) {
        std::cout << "GGML model initialized successfully: " << modelInfo_.id << std::endl;
    } else {
        std::cerr << "Failed to initialize GGML model: " << modelInfo_.id << std::endl;
    }
    
    return initialized_;
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
        std::string output;
        if (engine_->processInput(text, output)) {
            result.text = output;
            result.success = true;
        } else {
            result.success = false;
            result.errorMessage = "Translation failed";
        }
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
    return lastStats_;
}

} // namespace models
} // namespace koebridge
