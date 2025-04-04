/**
 * @file audio_capture.cc
 * @brief Implementation of the AudioCapture class for audio input capture
 */

#include "audio_capture.h"
#include <stdexcept>
#include <cstring>

AudioCapture::AudioCapture(int sampleRate, int channels, int framesPerBuffer)
    : stream_(nullptr)
    , sampleRate_(sampleRate)
    , channels_(channels)
    , framesPerBuffer_(framesPerBuffer)
    , selectedDevice_(-1)
{
    // Initialize PortAudio
    PaError err = Pa_Initialize();
    if (err != paNoError) {
        lastError_ = std::string("Failed to initialize PortAudio: ") + Pa_GetErrorText(err);
        throw std::runtime_error(lastError_);
    }
}

AudioCapture::~AudioCapture() {
    stop();
    Pa_Terminate();
}

std::vector<AudioDeviceInfo> AudioCapture::getInputDevices() const {
    std::vector<AudioDeviceInfo> devices;
    int numDevices = Pa_GetDeviceCount();
    
    for (int i = 0; i < numDevices; ++i) {
        const PaDeviceInfo* deviceInfo = Pa_GetDeviceInfo(i);
        if (deviceInfo->maxInputChannels > 0) {
            AudioDeviceInfo info;
            info.index = i;
            info.name = deviceInfo->name;
            info.maxInputChannels = deviceInfo->maxInputChannels;
            info.maxOutputChannels = deviceInfo->maxOutputChannels;
            info.defaultSampleRate = deviceInfo->defaultSampleRate;
            devices.push_back(info);
        }
    }
    
    return devices;
}

bool AudioCapture::selectInputDevice(int deviceIndex) {
    if (deviceIndex < 0 || deviceIndex >= Pa_GetDeviceCount()) {
        lastError_ = "Invalid device index";
        return false;
    }
    
    const PaDeviceInfo* deviceInfo = Pa_GetDeviceInfo(deviceIndex);
    if (deviceInfo->maxInputChannels == 0) {
        lastError_ = "Selected device has no input channels";
        return false;
    }
    
    selectedDevice_ = deviceIndex;
    return true;
}

bool AudioCapture::start() {
    if (selectedDevice_ == -1) {
        lastError_ = "No input device selected";
        return false;
    }
    
    PaStreamParameters inputParameters;
    inputParameters.device = selectedDevice_;
    inputParameters.channelCount = channels_;
    inputParameters.sampleFormat = paFloat32;
    inputParameters.suggestedLatency = Pa_GetDeviceInfo(selectedDevice_)->defaultLowInputLatency;
    inputParameters.hostApiSpecificStreamInfo = nullptr;
    
    PaError err = Pa_OpenStream(&stream_,
                               &inputParameters,
                               nullptr,
                               sampleRate_,
                               framesPerBuffer_,
                               paClipOff,
                               paCallback,
                               this);
    
    if (err != paNoError) {
        lastError_ = std::string("Failed to open stream: ") + Pa_GetErrorText(err);
        return false;
    }
    
    err = Pa_StartStream(stream_);
    if (err != paNoError) {
        lastError_ = std::string("Failed to start stream: ") + Pa_GetErrorText(err);
        Pa_CloseStream(stream_);
        stream_ = nullptr;
        return false;
    }
    
    return true;
}

void AudioCapture::stop() {
    if (stream_) {
        Pa_StopStream(stream_);
        Pa_CloseStream(stream_);
        stream_ = nullptr;
    }
}

void AudioCapture::setAudioCallback(std::function<void(const float*, int)> callback) {
    audioCallback_ = std::move(callback);
}

std::string AudioCapture::getLastError() const {
    return lastError_;
}

int AudioCapture::paCallback(const void* inputBuffer, void* outputBuffer,
                           unsigned long framesPerBuffer,
                           const PaStreamCallbackTimeInfo* timeInfo,
                           PaStreamCallbackFlags statusFlags,
                           void* userData) {
    AudioCapture* capture = static_cast<AudioCapture*>(userData);
    
    if (statusFlags) {
        capture->lastError_ = "Stream callback error";
        return paComplete;
    }
    
    if (inputBuffer && capture->audioCallback_) {
        const float* input = static_cast<const float*>(inputBuffer);
        capture->audioCallback_(input, framesPerBuffer);
    }
    
    return paContinue;
}
