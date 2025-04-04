/**
 * @file audio_capture.h
 * @brief Header file for the AudioCapture class that handles audio input capture
 */

#pragma once

/**
 * @class AudioCapture
 * @brief A class that manages audio capture from input devices
 * 
 * This class provides functionality to capture audio from input devices
 * such as microphones or other audio sources.
 */
class AudioCapture {
public:
    /**
     * @brief Constructor for AudioCapture
     */
    AudioCapture();
    
    /**
     * @brief Destructor for AudioCapture
     */
    ~AudioCapture();
    
    /**
     * @brief Starts the audio capture process
     * @return bool True if capture started successfully, false otherwise
     */
    bool start();
    
    /**
     * @brief Stops the audio capture process
     */
    void stop();
    
private:
    // Implementation details
};
