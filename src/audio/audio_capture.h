/**
 * @file audio_capture.h
 * @brief Header file for the AudioCapture class that handles audio input capture
 */

#pragma once

#include <portaudio.h>
#include <vector>
#include <string>
#include <functional>

/**
 * @struct AudioDeviceInfo
 * @brief Information about an audio device
 */
struct AudioDeviceInfo {
    int index;              ///< Device index
    std::string name;       ///< Device name
    int maxInputChannels;   ///< Maximum number of input channels
    int maxOutputChannels;  ///< Maximum number of output channels
    double defaultSampleRate; ///< Default sample rate
};

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
     * @param sampleRate Sample rate for audio capture (default: 16000)
     * @param channels Number of channels (default: 1)
     * @param framesPerBuffer Number of frames per buffer (default: 1024)
     */
    AudioCapture(int sampleRate = 16000, int channels = 1, int framesPerBuffer = 1024);
    
    /**
     * @brief Destructor for AudioCapture
     */
    ~AudioCapture();
    
    /**
     * @brief Get list of available audio input devices
     * @return Vector of AudioDeviceInfo for available input devices
     */
    std::vector<AudioDeviceInfo> getInputDevices() const;
    
    /**
     * @brief Select an input device by index
     * @param deviceIndex Index of the device to select
     * @return bool True if device was selected successfully
     */
    bool selectInputDevice(int deviceIndex);
    
    /**
     * @brief Starts the audio capture process
     * @return bool True if capture started successfully, false otherwise
     */
    bool start();
    
    /**
     * @brief Stops the audio capture process
     */
    void stop();
    
    /**
     * @brief Set callback function for audio data
     * @param callback Function to be called with audio data
     */
    void setAudioCallback(std::function<void(const float*, int)> callback);
    
    /**
     * @brief Get current error message if any
     * @return std::string Error message
     */
    std::string getLastError() const;
    
private:
    /**
     * @brief PortAudio callback function
     * @param inputBuffer Input audio buffer
     * @param outputBuffer Output audio buffer (unused)
     * @param framesPerBuffer Number of frames in the buffer
     * @param timeInfo Timing information
     * @param statusFlags Status flags
     * @return int PaContinue or paComplete
     */
    static int paCallback(const void* inputBuffer, void* outputBuffer,
                         unsigned long framesPerBuffer,
                         const PaStreamCallbackTimeInfo* timeInfo,
                         PaStreamCallbackFlags statusFlags,
                         void* userData);
    
    PaStream* stream_;                    ///< PortAudio stream
    int sampleRate_;                      ///< Sample rate
    int channels_;                        ///< Number of channels
    int framesPerBuffer_;                 ///< Frames per buffer
    int selectedDevice_;                  ///< Selected device index
    std::string lastError_;               ///< Last error message
    std::function<void(const float*, int)> audioCallback_; ///< Audio callback function
};
