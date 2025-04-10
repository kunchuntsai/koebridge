/**
 * @file whisper_wrapper.h
 * @brief Header file for Whisper speech-to-text wrapper
 */

#pragma once

#include <string>
#include <vector>
#include <memory>
#include <functional>

struct whisper_context;  // Forward declaration

namespace koebridge {
namespace stt {

/**
 * @struct WhisperConfig
 * @brief Configuration options for Whisper model
 */
struct WhisperConfig {
    int n_threads = 4;           ///< Number of threads to use
    int n_max_text_ctx = 16384;  ///< Maximum text context
    bool translate = false;      ///< Whether to translate to English
    bool print_special = false;  ///< Whether to print special tokens
    bool print_progress = false; ///< Whether to print progress
    bool print_timestamps = true;///< Whether to print timestamps
    bool print_realtime = false; ///< Whether to print in real-time
    std::string language = "ja"; ///< Language code (default: Japanese)
};

/**
 * @struct TranscriptionResult
 * @brief Result of audio transcription
 */
struct TranscriptionResult {
    std::string text;           ///< Transcribed text
    bool success = false;       ///< Whether transcription was successful
    std::string error;          ///< Error message if failed
    float duration = 0.0f;      ///< Audio duration in seconds
    std::vector<float> timestamps; ///< Timestamps for each segment
};

/**
 * @class WhisperWrapper
 * @brief Wrapper class for the Whisper speech-to-text model
 *
 * This class provides a C++ interface to the Whisper speech recognition model,
 * handling model loading and audio transcription.
 */
class WhisperWrapper {
public:
    /**
     * @brief Constructor for WhisperWrapper
     * @param config Configuration options for the model
     */
    explicit WhisperWrapper(const WhisperConfig& config = WhisperConfig());

    /**
     * @brief Destructor for WhisperWrapper
     */
    ~WhisperWrapper();

    /**
     * @brief Load the Whisper model from a file
     * @param modelPath Path to the Whisper model file
     * @return bool True if model was loaded successfully, false otherwise
     */
    bool loadModel(const std::string& modelPath);

    /**
     * @brief Transcribe audio data to text
     * @param audioData Vector of audio samples to transcribe
     * @param sampleRate Sample rate of the audio data
     * @return TranscriptionResult The transcription result with metadata
     */
    TranscriptionResult transcribe(const std::vector<float>& audioData, int sampleRate = 16000);

    /**
     * @brief Set configuration options
     * @param config New configuration options
     */
    void setConfig(const WhisperConfig& config);

    /**
     * @brief Get current configuration
     * @return WhisperConfig Current configuration options
     */
    WhisperConfig getConfig() const;

    /**
     * @brief Check if model is loaded
     * @return bool True if model is loaded and ready
     */
    bool isModelLoaded() const;

    /**
     * @brief Set progress callback
     * @param callback Function to call with progress updates
     */
    void setProgressCallback(std::function<void(float)> callback);

private:
    whisper_context* context;
    WhisperConfig config;
    bool modelLoaded;
    std::function<void(float)> progressCallback;
};

} // namespace stt
} // namespace koebridge
