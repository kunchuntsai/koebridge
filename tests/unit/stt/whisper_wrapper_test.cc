#include <gtest/gtest.h>
#include "stt/whisper_wrapper.h"
#include <vector>
#include <string>

namespace koebridge {
namespace stt {
namespace testing {

class WhisperWrapperTest : public ::testing::Test {
protected:
    void SetUp() override {
        wrapper = std::make_unique<WhisperWrapper>();
    }

    void TearDown() override {
        wrapper.reset();
    }

    std::unique_ptr<WhisperWrapper> wrapper;
};

TEST_F(WhisperWrapperTest, Initialization) {
    EXPECT_FALSE(wrapper->isModelLoaded());
    EXPECT_EQ(wrapper->getConfig().language, "ja");
    EXPECT_EQ(wrapper->getConfig().n_threads, 4);
}

TEST_F(WhisperWrapperTest, LoadModel) {
    // Test with non-existent model
    EXPECT_FALSE(wrapper->loadModel("nonexistent_model.bin"));
    EXPECT_FALSE(wrapper->isModelLoaded());

    // Test with valid model (assuming model exists in test data)
    EXPECT_TRUE(wrapper->loadModel("test_data/whisper-tiny.bin"));
    EXPECT_TRUE(wrapper->isModelLoaded());
}

TEST_F(WhisperWrapperTest, TranscribeEmptyAudio) {
    std::vector<float> emptyAudio;
    auto result = wrapper->transcribe(emptyAudio);
    EXPECT_FALSE(result.success);
    EXPECT_EQ(result.error, "Empty audio data");
}

TEST_F(WhisperWrapperTest, TranscribeWithoutModel) {
    std::vector<float> audioData(16000, 0.0f); // 1 second of silence
    auto result = wrapper->transcribe(audioData);
    EXPECT_FALSE(result.success);
    EXPECT_EQ(result.error, "Model not loaded");
}

TEST_F(WhisperWrapperTest, TranscribeWithModel) {
    // Load model first
    ASSERT_TRUE(wrapper->loadModel("test_data/whisper-tiny.bin"));

    // Create test audio data (1 second of silence)
    std::vector<float> audioData(16000, 0.0f);

    // Set up progress callback
    float progress = 0.0f;
    wrapper->setProgressCallback([&progress](float p) { progress = p; });

    // Perform transcription
    auto result = wrapper->transcribe(audioData);
    EXPECT_TRUE(result.success);
    EXPECT_TRUE(result.text.empty()); // Should be empty for silence
    EXPECT_EQ(result.duration, 1.0f);
    EXPECT_GE(progress, 0.0f);
    EXPECT_LE(progress, 1.0f);
}

TEST_F(WhisperWrapperTest, Configuration) {
    WhisperConfig config;
    config.n_threads = 2;
    config.language = "en";
    config.translate = true;

    wrapper->setConfig(config);
    auto newConfig = wrapper->getConfig();
    EXPECT_EQ(newConfig.n_threads, 2);
    EXPECT_EQ(newConfig.language, "en");
    EXPECT_TRUE(newConfig.translate);
}

} // namespace testing
} // namespace stt
} // namespace koebridge