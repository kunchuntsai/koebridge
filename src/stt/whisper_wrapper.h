#pragma once

#include <string>
#include <vector>

class WhisperWrapper {
public:
    WhisperWrapper();
    ~WhisperWrapper();
    
    bool loadModel(const std::string& modelPath);
    std::string transcribe(const std::vector<float>& audioData);
    
private:
    void* context; // Will point to whisper_context
};
