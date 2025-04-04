#include "audio_processor.h"

AudioProcessor::AudioProcessor() {
    // Initialize processor
}

void AudioProcessor::process(const std::vector<float>& input, std::vector<float>& output) {
    // Process audio data
    output = input; // Placeholder passthrough
}
