/**
 * @file data_structures.h
 * @brief Core data structures for translation functionality
 */

#pragma once

#include <string>
#include <map>
#include <vector>
#include <chrono>
#include <filesystem>
#include <functional>

namespace koebridge {
namespace translation {

/**
 * @enum ModelType
 * @brief Enumeration of supported model types
 */
enum class ModelType {
    GGML,      ///< GGML model format
    ONNX,      ///< ONNX model format
    TENSORRT,  ///< TensorRT model format
    CUSTOM     ///< Custom model format
};

/**
 * @struct ModelInfo
 * @brief Information about a translation model
 */
struct ModelInfo {
    std::string id;                    ///< Unique identifier for the model
    std::string path;                  ///< Path to the model file
    size_t size;                       ///< Size of the model file in bytes
    std::time_t lastModified;          ///< Last modification time
};

/**
 * @struct TranslationOptions
 * @brief Configuration options for translation operations
 */
struct TranslationOptions {
    enum class Style {
        NATURAL,
        FORMAL,
        CASUAL
    };
    
    float temperature = 0.7f;          ///< Sampling temperature
    int maxLength = 1024;              ///< Maximum sequence length
    int beamSize = 4;                  ///< Beam search size
    Style style = Style::NATURAL;      ///< Translation style
    int timeoutMs = 30000;             ///< Operation timeout in milliseconds
};

/**
 * @struct InferenceStats
 * @brief Statistics about model inference operations
 */
struct InferenceStats {
    double totalTimeMs = 0.0;        ///< Total processing time in milliseconds
    double inferenceTimeMs = 0.0;    ///< Model inference time in milliseconds
    int inputTokenCount = 0;         ///< Number of input tokens
    int outputTokenCount = 0;        ///< Number of output tokens
};

/**
 * @struct TranslationResult
 * @brief Result of a translation operation
 */
struct TranslationResult {
    std::string sourceText;            ///< Original text to translate
    std::string text;                  ///< Translated text
    bool success = false;              ///< Whether translation was successful
    std::string errorMessage;          ///< Error message if translation failed
    struct {
        double totalTimeMs = 0.0;      ///< Total processing time
        double inferenceTimeMs = 0.0;  ///< Time spent in inference
        double preprocessTimeMs = 0.0; ///< Time spent in preprocessing
        double postprocessTimeMs = 0.0;///< Time spent in postprocessing
        size_t inputTokenCount = 0;    ///< Number of input tokens
        size_t outputTokenCount = 0;   ///< Number of output tokens
    } metrics;
};

using ProgressCallback = std::function<void(int progress, const std::string& message)>;

} // namespace translation
} // namespace koebridge
