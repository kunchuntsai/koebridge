#pragma once

#include <string>
#include <map>
#include <vector>

enum class ModelType {
    GGML,
    ONNX,
    TENSORRT,
    CUSTOM
};

struct ModelInfo {
    std::string id;              // Unique identifier
    std::string name;            // Display name
    std::string version;         // Version string
    std::string filePath;        // Path to model file
    int64_t sizeBytes;           // Model size in bytes
    ModelType type;              // GGML, ONNX, etc.
    std::map<std::string, std::string> metadata; // Additional info
};

struct TranslationOptions {
    float temperature = 0.7f;    // Sampling temperature
    int maxLength = 1024;        // Maximum output length
    int beamSize = 4;            // Beam search size
    bool streamOutput = false;   // Stream partial results
    
    enum class Style {           // Translation style
        LITERAL,
        NATURAL,
        FORMAL,
        CASUAL
    } style = Style::NATURAL;
    
    int timeoutMs = 30000;       // Translation timeout
};

struct InferenceStats {
    double totalTimeMs = 0.0;
    double inferenceTimeMs = 0.0;
    int inputTokenCount = 0;
    int outputTokenCount = 0;
};

struct TranslationResult {
    std::string sourceText;      // Original Japanese text
    std::string translatedText;  // Translated English text
    bool success = false;        // Success flag
    std::string errorMessage;    // Error message if any
    
    struct Metrics {
        double totalTimeMs = 0.0;      // Total processing time
        double inferenceTimeMs = 0.0;  // Model inference time
        double preprocessTimeMs = 0.0; // Pre-processing time
        double postprocessTimeMs = 0.0;// Post-processing time
        int inputTokenCount = 0;     // Number of input tokens
        int outputTokenCount = 0;    // Number of output tokens
    } metrics;
};
