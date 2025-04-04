#pragma once

#include <string>
#include <vector>
#include <memory>
#include "translation/data_structures.h"

class InferenceEngine {
public:
    InferenceEngine();
    ~InferenceEngine();

    bool initialize(const std::string& modelPath, const std::map<std::string, std::string>& config);
    bool isInitialized() const;
    
    // Core inference operations
    std::vector<int> runInference(
        const std::vector<int>& inputTokens, 
        const TranslationOptions& options, 
        InferenceStats& stats
    );
    
    // Tokenization helpers
    std::vector<int> tokenize(const std::string& text);
    std::string detokenize(const std::vector<int>& tokens);
    
private:
    class Impl;
    std::unique_ptr<Impl> pImpl_;
    bool initialized_ = false;
};
