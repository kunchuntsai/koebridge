/**
 * @file data_structures.h
 * @brief Core data structures for translation functionality
 */

#pragma once

#include <string>
#include <map>
#include <vector>

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
    std::string id;              ///< Unique identifier for the model
    std::string name;            ///< Display name of the model
    std::string version;         ///< Version string of the model
    std::string filePath;        ///< Path to the model file
    int64_t sizeBytes;           ///< Size of the model in bytes
    ModelType type;              ///< Type of the model (GGML, ONNX, etc.)
    std::map<std::string, std::string> metadata; ///< Additional model metadata
};

/**
 * @struct TranslationOptions
 * @brief Configuration options for translation operations
 */
struct TranslationOptions {
    float temperature = 0.7f;    ///< Sampling temperature for text generation
    int maxLength = 1024;        ///< Maximum output length in tokens
    int beamSize = 4;            ///< Beam search size for translation
    bool streamOutput = false;   ///< Whether to stream partial results
    
    /**
     * @enum Style
     * @brief Translation style options
     */
    enum class Style {
        LITERAL,    ///< Literal translation
        NATURAL,    ///< Natural language translation
        FORMAL,     ///< Formal language translation
        CASUAL      ///< Casual language translation
    } style = Style::NATURAL;
    
    int timeoutMs = 30000;       ///< Translation timeout in milliseconds
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
    std::string sourceText;      ///< Original Japanese text
    std::string translatedText;  ///< Translated English text
    bool success = false;        ///< Whether translation was successful
    std::string errorMessage;    ///< Error message if translation failed
    
    /**
     * @struct Metrics
     * @brief Detailed metrics about the translation process
     */
    struct Metrics {
        double totalTimeMs = 0.0;      ///< Total processing time in milliseconds
        double inferenceTimeMs = 0.0;  ///< Model inference time in milliseconds
        double preprocessTimeMs = 0.0; ///< Pre-processing time in milliseconds
        double postprocessTimeMs = 0.0;///< Post-processing time in milliseconds
        int inputTokenCount = 0;       ///< Number of input tokens
        int outputTokenCount = 0;      ///< Number of output tokens
    } metrics;
};
