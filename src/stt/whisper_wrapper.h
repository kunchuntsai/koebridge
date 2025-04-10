/**
 * @file whisper_wrapper.h
 * @brief Header file for Whisper speech-to-text wrapper
 */

#pragma once

#include <string>
#include <vector>

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
     */
    WhisperWrapper();

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
     * @return std::string The transcribed text
     */
    std::string transcribe(const std::vector<float>& audioData);

private:
    void* context; ///< Pointer to the Whisper context
};
