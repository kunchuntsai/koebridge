/**
 * @file audio_capture_test.cc
 * @brief Unit tests for the AudioCapture class
 */

#include <gtest/gtest.h>
#include "audio/audio_capture.h"
#include <thread>
#include <chrono>
#include <atomic>

class AudioCaptureTest : public ::testing::Test {
protected:
    void SetUp() override {
        capture = std::make_unique<AudioCapture>(16000, 1, 1024);
    }

    void TearDown() override {
        capture.reset();
    }

    std::unique_ptr<AudioCapture> capture;
};

// Test device enumeration
TEST_F(AudioCaptureTest, GetInputDevices) {
    auto devices = capture->getInputDevices();
    EXPECT_FALSE(devices.empty()) << "No input devices found";
    
    // Print available devices for debugging
    std::cout << "\nAvailable input devices:" << std::endl;
    for (const auto& device : devices) {
        std::cout << "Device " << device.index << ": " << device.name 
                  << " (Channels: " << device.maxInputChannels 
                  << ", Sample Rate: " << device.defaultSampleRate << ")" << std::endl;
    }
}

// Test device selection
TEST_F(AudioCaptureTest, SelectInputDevice) {
    auto devices = capture->getInputDevices();
    ASSERT_FALSE(devices.empty()) << "No input devices found";
    
    // Try to select the first input device
    EXPECT_TRUE(capture->selectInputDevice(devices[0].index));
    
    // Try to select an invalid device
    EXPECT_FALSE(capture->selectInputDevice(-1));
    EXPECT_FALSE(capture->selectInputDevice(9999));
}

// Test audio capture
TEST_F(AudioCaptureTest, AudioCapture) {
    auto devices = capture->getInputDevices();
    ASSERT_FALSE(devices.empty()) << "No input devices found";
    
    // Select first input device
    ASSERT_TRUE(capture->selectInputDevice(devices[0].index));
    
    // Counter for received audio frames
    std::atomic<int> frameCount{0};
    
    // Set up audio callback
    capture->setAudioCallback([&frameCount](const float* buffer, int frames) {
        frameCount += frames;
    });
    
    // Start capture
    ASSERT_TRUE(capture->start()) << "Failed to start capture: " << capture->getLastError();
    
    // Capture audio for 2 seconds
    std::this_thread::sleep_for(std::chrono::seconds(2));
    
    // Stop capture
    capture->stop();
    
    // Verify we received some audio data
    EXPECT_GT(frameCount, 0) << "No audio frames received";
    std::cout << "\nReceived " << frameCount << " audio frames" << std::endl;
}

// Test error handling
TEST_F(AudioCaptureTest, ErrorHandling) {
    // Try to start without selecting a device
    EXPECT_FALSE(capture->start());
    EXPECT_FALSE(capture->getLastError().empty());
    
    // Try to select invalid device
    EXPECT_FALSE(capture->selectInputDevice(-1));
    EXPECT_FALSE(capture->getLastError().empty());
}

// Test cleanup
TEST_F(AudioCaptureTest, Cleanup) {
    auto devices = capture->getInputDevices();
    ASSERT_FALSE(devices.empty()) << "No input devices found";
    
    // Select and start capture
    ASSERT_TRUE(capture->selectInputDevice(devices[0].index));
    ASSERT_TRUE(capture->start());
    
    // Destructor should clean up properly
    capture.reset();
    
    // Create new instance to verify PortAudio is still working
    capture = std::make_unique<AudioCapture>();
    EXPECT_FALSE(capture->getInputDevices().empty());
} 