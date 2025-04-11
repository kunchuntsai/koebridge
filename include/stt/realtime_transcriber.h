/**
 * @file realtime_transcriber.h
 * @brief Header file for real-time audio transcription using Whisper
 */

#pragma once

#include <memory>
#include <string>
#include <vector>
#include <functional>
#include <mutex>
#include <queue>
#include <thread>
#include <atomic>

#include "audio/audio_capture.h"
#include "stt/whisper_wrapper.h"

namespace koebridge {
namespace stt {

/**
 * @struct TranscriptionConfig
 * @brief Configuration for real-time transcription
 */
struct TranscriptionConfig {
    int sampleRate = 16000;           ///< Sample rate for audio capture
    int channels = 1;                 ///< Number of audio channels
    int framesPerBuffer = 1024;       ///< Frames per buffer
    int bufferDurationMs = 3000;      ///< Duration of audio buffer in milliseconds
    bool translate = false;           ///< Whether to translate to English
    std::string language = "ja";      ///< Language code (default: Japanese)
    int nThreads = 4;                 ///< Number of threads for Whisper
};

/**
 * @class RealtimeTranscriber
 * @brief Class for real-time audio transcription using Whisper
 *
 * This class integrates AudioCapture with WhisperWrapper to provide
 * real-time transcription of audio input.
 */
class RealtimeTranscriber {
public:
    /**
     * @brief Constructor for RealtimeTranscriber
     * @param config Configuration options for transcription
     */
    explicit RealtimeTranscriber(const TranscriptionConfig& config = TranscriptionConfig());

    /**
     * @brief Destructor for RealtimeTranscriber
     */
    ~RealtimeTranscriber();

    /**
     * @brief Initialize the transcriber with a Whisper model
     * @param modelPath Path to the Whisper model file
     * @return bool True if initialization was successful
     */
    bool initialize(const std::string& modelPath);

    /**
     * @brief Start real-time transcription
     * @param deviceIndex Index of the audio input device to use
     * @return bool True if transcription started successfully
     */
    bool start(int deviceIndex);

    /**
     * @brief Stop real-time transcription
     */
    void stop();

    /**
     * @brief Set callback for transcription results
     * @param callback Function to be called with transcription results
     */
    void setTranscriptionCallback(std::function<void(const TranscriptionResult&)> callback);

    /**
     * @brief Get current error message if any
     * @return std::string Error message
     */
    std::string getLastError() const;

    /**
     * @brief Get list of available audio input devices
     * @return Vector of AudioDeviceInfo for available input devices
     */
    std::vector<AudioDeviceInfo> getInputDevices() const;

private:
    /**
     * @brief Process audio data from the capture callback
     * @param buffer Audio data buffer
     * @param frames Number of frames in the buffer
     */
    void processAudioData(const float* buffer, int frames);

    /**
     * @brief Worker thread function for processing audio buffers
     */
    void processAudioBuffer();

    std::unique_ptr<AudioCapture> audioCapture_;
    std::unique_ptr<WhisperWrapper> whisperWrapper_;
    TranscriptionConfig config_;
    std::string lastError_;

    std::vector<float> audioBuffer_;
    size_t bufferPosition_;
    std::mutex bufferMutex_;

    std::thread processingThread_;
    std::atomic<bool> isRunning_;
    std::atomic<bool> shouldStop_;

    std::function<void(const TranscriptionResult&)> transcriptionCallback_;
};

} // namespace stt
} // namespace koebridge