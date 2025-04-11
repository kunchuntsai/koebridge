/**
 * @file realtime_transcriber.cc
 * @brief Implementation of real-time audio transcription using Whisper
 */

#include "stt/realtime_transcriber.h"
#include "utils/logger.h"
#include <algorithm>
#include <chrono>
#include <thread>

namespace koebridge {
namespace stt {

RealtimeTranscriber::RealtimeTranscriber(const TranscriptionConfig& config)
    : config_(config)
    , bufferPosition_(0)
    , isRunning_(false)
    , shouldStop_(false)
    , transcriptionCallback_(nullptr) {

    // Initialize audio capture
    audioCapture_ = std::make_unique<AudioCapture>(
        config.sampleRate,
        config.channels,
        config.framesPerBuffer
    );

    // Calculate buffer size based on duration
    size_t bufferSize = (config.sampleRate * config.bufferDurationMs) / 1000;
    audioBuffer_.resize(bufferSize);

    // Initialize Whisper wrapper with default config
    WhisperConfig whisperConfig;
    whisperConfig.n_threads = config.nThreads;
    whisperConfig.translate = config.translate;
    whisperConfig.language = config.language;
    whisperWrapper_ = std::make_unique<WhisperWrapper>(whisperConfig);
}

RealtimeTranscriber::~RealtimeTranscriber() {
    stop();
}

bool RealtimeTranscriber::initialize(const std::string& modelPath) {
    if (!whisperWrapper_->loadModel(modelPath)) {
        lastError_ = "Failed to load Whisper model: " + modelPath;
        LOG_ERROR(lastError_);
        return false;
    }
    return true;
}

bool RealtimeTranscriber::start(int deviceIndex) {
    if (isRunning_) {
        lastError_ = "Transcription is already running";
        LOG_ERROR(lastError_);
        return false;
    }

    if (!whisperWrapper_->isModelLoaded()) {
        lastError_ = "Whisper model not loaded";
        LOG_ERROR(lastError_);
        return false;
    }

    // Select audio device
    if (!audioCapture_->selectInputDevice(deviceIndex)) {
        lastError_ = "Failed to select audio device: " + audioCapture_->getLastError();
        LOG_ERROR(lastError_);
        return false;
    }

    // Set up audio callback
    audioCapture_->setAudioCallback([this](const float* buffer, int frames) {
        this->processAudioData(buffer, frames);
    });

    // Start audio capture
    if (!audioCapture_->start()) {
        lastError_ = "Failed to start audio capture: " + audioCapture_->getLastError();
        LOG_ERROR(lastError_);
        return false;
    }

    // Start processing thread
    shouldStop_ = false;
    isRunning_ = true;
    processingThread_ = std::thread(&RealtimeTranscriber::processAudioBuffer, this);

    LOG_INFO("Started real-time transcription");
    return true;
}

void RealtimeTranscriber::stop() {
    if (!isRunning_) {
        return;
    }

    shouldStop_ = true;
    isRunning_ = false;

    // Stop audio capture
    audioCapture_->stop();

    // Wait for processing thread to finish
    if (processingThread_.joinable()) {
        processingThread_.join();
    }

    LOG_INFO("Stopped real-time transcription");
}

void RealtimeTranscriber::setTranscriptionCallback(std::function<void(const TranscriptionResult&)> callback) {
    transcriptionCallback_ = std::move(callback);
}

std::string RealtimeTranscriber::getLastError() const {
    return lastError_;
}

void RealtimeTranscriber::processAudioData(const float* buffer, int frames) {
    if (!isRunning_ || !buffer || frames <= 0) {
        return;
    }

    std::lock_guard<std::mutex> lock(bufferMutex_);

    // Copy audio data to circular buffer
    for (int i = 0; i < frames; ++i) {
        audioBuffer_[bufferPosition_] = buffer[i];
        bufferPosition_ = (bufferPosition_ + 1) % audioBuffer_.size();
    }
}

void RealtimeTranscriber::processAudioBuffer() {
    std::vector<float> processingBuffer;
    processingBuffer.reserve(audioBuffer_.size());

    while (!shouldStop_) {
        {
            std::lock_guard<std::mutex> lock(bufferMutex_);

            // Copy current buffer contents for processing
            processingBuffer.clear();
            size_t start = (bufferPosition_ + 1) % audioBuffer_.size();
            size_t end = bufferPosition_;

            if (start <= end) {
                processingBuffer.insert(processingBuffer.end(),
                    audioBuffer_.begin() + start,
                    audioBuffer_.begin() + end + 1);
            } else {
                processingBuffer.insert(processingBuffer.end(),
                    audioBuffer_.begin() + start,
                    audioBuffer_.end());
                processingBuffer.insert(processingBuffer.end(),
                    audioBuffer_.begin(),
                    audioBuffer_.begin() + end + 1);
            }
        }

        // Process audio buffer if we have enough data
        if (processingBuffer.size() >= config_.sampleRate) {  // Process at least 1 second of audio
            TranscriptionResult result = whisperWrapper_->transcribe(processingBuffer, config_.sampleRate);

            if (result.success && transcriptionCallback_) {
                transcriptionCallback_(result);
            }
        }

        // Sleep for a short duration to prevent busy waiting
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
}

std::vector<AudioDeviceInfo> RealtimeTranscriber::getInputDevices() const {
    return audioCapture_->getInputDevices();
}

} // namespace stt
} // namespace koebridge