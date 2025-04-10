#include "whisper_wrapper.h"
#include "utils/logger.h"
#include "whisper.h"
#include <algorithm>
#include <cstring>
#include <stdexcept>
#include <sstream>

namespace koebridge {
namespace stt {

WhisperWrapper::WhisperWrapper(const WhisperConfig& config)
    : context(nullptr)
    , config(config)
    , modelLoaded(false)
    , progressCallback(nullptr) {
    // Validate configuration
    if (config.n_threads < 1) {
        std::stringstream ss;
        ss << "Invalid thread count (" << config.n_threads << "), setting to 1";
        LOG_WARNING(ss.str());
        this->config.n_threads = 1;
    }
    if (config.n_max_text_ctx < 1) {
        std::stringstream ss;
        ss << "Invalid text context size (" << config.n_max_text_ctx << "), setting to default";
        LOG_WARNING(ss.str());
        this->config.n_max_text_ctx = 16384;
    }
}

WhisperWrapper::~WhisperWrapper() {
    if (context) {
        whisper_free(context);
        context = nullptr;
    }
}

bool WhisperWrapper::loadModel(const std::string& modelPath) {
    if (modelPath.empty()) {
        LOG_ERROR("Model path is empty");
        return false;
    }

    // Free existing context if any
    if (context) {
        whisper_free(context);
        context = nullptr;
        modelLoaded = false;
    }

    // Initialize parameters with defaults
    whisper_context_params params = whisper_context_default_params();

    // Load new model with parameters
    context = whisper_init_from_file_with_params(modelPath.c_str(), params);
    if (!context) {
        std::stringstream ss;
        ss << "Failed to load Whisper model from: " << modelPath;
        LOG_ERROR(ss.str());
        return false;
    }

    modelLoaded = true;
    std::stringstream ss;
    ss << "Successfully loaded Whisper model from: " << modelPath;
    LOG_INFO(ss.str());
    return true;
}

TranscriptionResult WhisperWrapper::transcribe(const std::vector<float>& audioData, int sampleRate) {
    TranscriptionResult result;

    if (!modelLoaded || !context) {
        result.error = "Model not loaded";
        result.success = false;
        return result;
    }

    if (audioData.empty()) {
        result.error = "Empty audio data";
        result.success = false;
        return result;
    }

    if (sampleRate <= 0) {
        result.error = "Invalid sample rate";
        result.success = false;
        return result;
    }

    // Set up Whisper parameters
    whisper_full_params params = whisper_full_default_params(WHISPER_SAMPLING_GREEDY);
    params.n_threads = config.n_threads;
    params.print_special = config.print_special;
    params.print_progress = config.print_progress;
    params.print_timestamps = config.print_timestamps;
    params.print_realtime = config.print_realtime;
    params.translate = config.translate;
    params.language = config.language.c_str();
    params.n_max_text_ctx = config.n_max_text_ctx;

    // Set up progress callback if provided
    if (progressCallback) {
        params.new_segment_callback = [](struct whisper_context* /*ctx*/, struct whisper_state* /*state*/, int n_new, void* user_data) {
            auto* wrapper = static_cast<WhisperWrapper*>(user_data);
            if (wrapper && wrapper->progressCallback) {
                try {
                    float progress = static_cast<float>(n_new) / 100.0f;  // Convert to percentage
                    wrapper->progressCallback(progress);
                } catch (const std::exception& e) {
                    std::stringstream ss;
                    ss << "Progress callback error: " << e.what();
                    LOG_ERROR(ss.str());
                }
            }
        };
        params.new_segment_callback_user_data = this;
    }

    // Run inference
    if (whisper_full(context, params, audioData.data(), audioData.size()) != 0) {
        result.error = "Failed to run Whisper inference";
        result.success = false;
        return result;
    }

    // Get number of segments
    const int n_segments = whisper_full_n_segments(context);
    if (n_segments < 0) {
        result.error = "Invalid segment count from Whisper";
        result.success = false;
        return result;
    }

    result.timestamps.resize(n_segments * 2); // Start and end times for each segment

    // Extract text and timestamps
    std::string text;
    for (int i = 0; i < n_segments; ++i) {
        const char* segment_text = whisper_full_get_segment_text(context, i);
        if (!segment_text) {
            std::stringstream ss;
            ss << "Null segment text at index " << i;
            LOG_WARNING(ss.str());
            continue;
        }

        const float t0 = whisper_full_get_segment_t0(context, i);
        const float t1 = whisper_full_get_segment_t1(context, i);

        result.timestamps[i * 2] = t0;
        result.timestamps[i * 2 + 1] = t1;

        if (i > 0) text += " ";
        text += segment_text;
    }

    result.text = text;
    result.success = true;
    result.duration = audioData.size() / static_cast<float>(sampleRate);

    return result;
}

void WhisperWrapper::setConfig(const WhisperConfig& config) {
    this->config = config;
}

WhisperConfig WhisperWrapper::getConfig() const {
    return config;
}

bool WhisperWrapper::isModelLoaded() const {
    return modelLoaded;
}

void WhisperWrapper::setProgressCallback(std::function<void(float)> callback) {
    progressCallback = std::move(callback);
}

} // namespace stt
} // namespace koebridge
