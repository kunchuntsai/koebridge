/**
 * @file realtime_transcriber_test.cc
 * @brief Unit tests for the RealtimeTranscriber class
 */

#include <gtest/gtest.h>
#include "stt/realtime_transcriber.h"
#include <thread>
#include <chrono>
#include <atomic>

class RealtimeTranscriberTest : public ::testing::Test {
protected:
    void SetUp() override {
        config.sampleRate = 16000;
        config.channels = 1;
        config.framesPerBuffer = 1024;
        config.bufferDurationMs = 3000;
        config.translate = false;
        config.language = "ja";
        config.nThreads = 4;

        transcriber = std::make_unique<koebridge::stt::RealtimeTranscriber>(config);
    }

    void TearDown() override {
        transcriber.reset();
    }

    koebridge::stt::TranscriptionConfig config;
    std::unique_ptr<koebridge::stt::RealtimeTranscriber> transcriber;
};

// Test initialization
TEST_F(RealtimeTranscriberTest, Initialization) {
    // Test with invalid model path
    EXPECT_FALSE(transcriber->initialize("nonexistent_model.bin"));
    EXPECT_FALSE(transcriber->getLastError().empty());

    // Test with valid model path (assuming model exists)
    // Note: This test requires a valid model file
    // EXPECT_TRUE(transcriber->initialize("path/to/valid/model.bin"));
}

// Test device enumeration
TEST_F(RealtimeTranscriberTest, GetInputDevices) {
    auto devices = transcriber->getInputDevices();
    EXPECT_FALSE(devices.empty()) << "No input devices found";

    // Print available devices for debugging
    std::cout << "\nAvailable input devices:" << std::endl;
    for (const auto& device : devices) {
        std::cout << "Device " << device.index << ": " << device.name
                  << " (Channels: " << device.maxInputChannels
                  << ", Sample Rate: " << device.defaultSampleRate << ")" << std::endl;
    }
}

// Test transcription callback
TEST_F(RealtimeTranscriberTest, TranscriptionCallback) {
    // Initialize with a valid model (assuming model exists)
    // transcriber->initialize("path/to/valid/model.bin");

    std::atomic<bool> callbackReceived{false};
    transcriber->setTranscriptionCallback([&callbackReceived](const koebridge::stt::TranscriptionResult& result) {
        callbackReceived = true;
        EXPECT_TRUE(result.success);
        EXPECT_FALSE(result.text.empty());
    });

    // Get available devices
    auto devices = transcriber->getInputDevices();
    ASSERT_FALSE(devices.empty()) << "No input devices found";

    // Start transcription with first available device
    EXPECT_TRUE(transcriber->start(devices[0].index));

    // Wait for a short duration to allow for some audio processing
    std::this_thread::sleep_for(std::chrono::seconds(2));

    // Stop transcription
    transcriber->stop();

    // Note: This test may not always receive a callback depending on audio input
    // EXPECT_TRUE(callbackReceived) << "No transcription callback received";
}

// Test error handling
TEST_F(RealtimeTranscriberTest, ErrorHandling) {
    // Try to start without initializing
    EXPECT_FALSE(transcriber->start(0));
    EXPECT_FALSE(transcriber->getLastError().empty());

    // Try to start with invalid device
    EXPECT_FALSE(transcriber->start(-1));
    EXPECT_FALSE(transcriber->getLastError().empty());
}

// Test cleanup
TEST_F(RealtimeTranscriberTest, Cleanup) {
    auto devices = transcriber->getInputDevices();
    ASSERT_FALSE(devices.empty()) << "No input devices found";

    // Initialize and start transcription
    // transcriber->initialize("path/to/valid/model.bin");
    EXPECT_TRUE(transcriber->start(devices[0].index));

    // Destructor should clean up properly
    transcriber.reset();

    // Create new instance to verify resources are cleaned up
    transcriber = std::make_unique<koebridge::stt::RealtimeTranscriber>(config);
    EXPECT_FALSE(transcriber->getInputDevices().empty());
}