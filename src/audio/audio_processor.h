/**
 * @file audio_processor.h
 * @brief Header file for audio signal processing
 */

#pragma once

#include <vector>

/**
 * @class AudioProcessor
 * @brief Class for processing audio signals
 *
 * This class provides functionality for processing audio signals, including
 * filtering, normalization, and other audio processing operations.
 */
class AudioProcessor {
public:
    /**
     * @brief Constructor for AudioProcessor
     */
    AudioProcessor();

    /**
     * @brief Process audio data from input buffer to output buffer
     * @param input Reference to input audio buffer containing raw audio data
     * @param output Reference to output buffer where processed audio will be stored
     */
    void process(const std::vector<float>& input, std::vector<float>& output);

private:
    // Implementation details
};
