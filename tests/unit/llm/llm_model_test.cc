#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <memory>
#include <string>
#include <fstream>
#include <filesystem>
#include "llm/llm_model.h"
#include "translation/data_structures.h"

namespace fs = std::filesystem;

namespace koebridge {
namespace llm {
namespace testing {

class LLMModelTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Create a temporary test directory
        testDir_ = fs::temp_directory_path() / "koebridge_llm_test";
        fs::create_directories(testDir_);

        // Create a dummy model file
        createDummyModel("test_llm", 4096);

        // Set up model info
        modelInfo_.id = "test_llm";
        modelInfo_.path = (testDir_ / "test_llm.bin").string();
        modelInfo_.size = 4096;
        modelInfo_.lastModified = std::time(nullptr);

        // Create default config
        config_.contextSize = 1024;
        config_.temperature = 0.5f;
        config_.numThreads = 2;

        // Create model instance
        model_ = std::make_unique<LLMModel>(modelInfo_, config_);
    }

    void TearDown() override {
        // Clean up the test directory
        fs::remove_all(testDir_);
    }

    void createDummyModel(const std::string& name, size_t size) {
        std::ofstream file(testDir_ / (name + ".bin"), std::ios::binary);
        std::vector<char> data(size, 0);
        file.write(data.data(), data.size());
        file.close();
    }

    fs::path testDir_;
    translation::ModelInfo modelInfo_;
    LLMConfig config_;
    std::unique_ptr<LLMModel> model_;
};

TEST_F(LLMModelTest, Initialization) {
    // Test initialization
    // In our mock implementation, this will always return true
    EXPECT_TRUE(model_->initialize());
    EXPECT_TRUE(model_->isInitialized());
}

TEST_F(LLMModelTest, TextCompletion) {
    // Initialize the model
    ASSERT_TRUE(model_->initialize());

    // Test text completion
    LLMOutput output = model_->complete("Hello, world!");

    // Verify the output
    EXPECT_TRUE(output.success);
    EXPECT_FALSE(output.text.empty());

    // Check that the stats have been populated
    EXPECT_GT(output.stats.totalTimeMs, 0.0);
    EXPECT_GT(output.stats.inferenceTimeMs, 0.0);
    EXPECT_GT(output.stats.inputTokenCount, 0);
    EXPECT_GT(output.stats.outputTokenCount, 0);
}

TEST_F(LLMModelTest, AsyncCompletion) {
    // Initialize the model
    ASSERT_TRUE(model_->initialize());

    // Test async text completion
    auto future = model_->completeAsync("Hello, world!");

    // Wait for the result
    LLMOutput output = future.get();

    // Verify the output
    EXPECT_TRUE(output.success);
    EXPECT_FALSE(output.text.empty());
}

TEST_F(LLMModelTest, ConfigurationChanges) {
    // Initialize with default config
    ASSERT_TRUE(model_->initialize());

    // Get the initial config
    LLMConfig initialConfig = model_->getConfig();
    EXPECT_EQ(initialConfig.contextSize, 1024);
    EXPECT_FLOAT_EQ(initialConfig.temperature, 0.5f);

    // Change the config
    LLMConfig newConfig = initialConfig;
    newConfig.contextSize = 2048;
    newConfig.temperature = 0.8f;

    model_->setConfig(newConfig);

    // Verify the config was changed
    LLMConfig updatedConfig = model_->getConfig();
    EXPECT_EQ(updatedConfig.contextSize, 2048);
    EXPECT_FLOAT_EQ(updatedConfig.temperature, 0.8f);
}

TEST_F(LLMModelTest, PromptFormatting) {
    // Initialize the model
    ASSERT_TRUE(model_->initialize());

    // Test different model types
    modelInfo_.modelType = "chatml";
    model_ = std::make_unique<LLMModel>(modelInfo_, config_);
    ASSERT_TRUE(model_->initialize());

    std::string prompt = "Hello, world!";
    std::string formattedPrompt = model_->formatPrompt(prompt);
    EXPECT_TRUE(formattedPrompt.find("<|im_start|>user") != std::string::npos);
    EXPECT_TRUE(formattedPrompt.find("<|im_end|>") != std::string::npos);
    EXPECT_TRUE(formattedPrompt.find("<|im_start|>assistant") != std::string::npos);
    EXPECT_TRUE(formattedPrompt.find(prompt) != std::string::npos);

    // Test Alpaca format
    modelInfo_.modelType = "alpaca";
    model_ = std::make_unique<LLMModel>(modelInfo_, config_);
    ASSERT_TRUE(model_->initialize());

    formattedPrompt = model_->formatPrompt(prompt);
    EXPECT_TRUE(formattedPrompt.find("### Instruction:") != std::string::npos);
    EXPECT_TRUE(formattedPrompt.find("### Response:") != std::string::npos);
    EXPECT_TRUE(formattedPrompt.find(prompt) != std::string::npos);

    // Test LLaMA format
    modelInfo_.modelType = "llama";
    model_ = std::make_unique<LLMModel>(modelInfo_, config_);
    ASSERT_TRUE(model_->initialize());

    formattedPrompt = model_->formatPrompt(prompt);
    EXPECT_TRUE(formattedPrompt.find("[INST]") != std::string::npos);
    EXPECT_TRUE(formattedPrompt.find("[/INST]") != std::string::npos);
    EXPECT_TRUE(formattedPrompt.find(prompt) != std::string::npos);
}

TEST_F(LLMModelTest, Tokenization) {
    // Initialize the model
    ASSERT_TRUE(model_->initialize());

    // Test tokenization
    std::string text = "Hello, world!";
    std::vector<int> tokens = model_->localTokenize(text);

    // Verify tokenization
    EXPECT_FALSE(tokens.empty());
    EXPECT_EQ(tokens.front(), 1);  // BOS token
    EXPECT_EQ(tokens.back(), 2);   // EOS token

    // Test detokenization
    std::string detokenized = model_->localDetokenize(tokens);

    // The detokenized text might not be exactly the same due to tokenization differences
    // but it should not be empty
    EXPECT_FALSE(detokenized.empty());
}

TEST_F(LLMModelTest, SamplingParameters) {
    // Initialize the model
    ASSERT_TRUE(model_->initialize());

    // Set different sampling parameters
    LLMConfig config = model_->getConfig();
    config.temperature = 0.0f;  // Deterministic sampling
    config.topK = 1;            // Only keep the most likely token
    config.topP = 0.1f;         // Very low nucleus sampling
    config.repeatPenalty = 2.0f; // Strong repeat penalty

    model_->setConfig(config);

    // Generate text with these parameters
    LLMOutput output = model_->complete("Hello, world!");

    // Verify the output
    EXPECT_TRUE(output.success);
    EXPECT_FALSE(output.text.empty());
}

TEST_F(LLMModelTest, FormatPromptChatML) {
    std::string prompt = "Hello, how are you?";
    modelInfo_.modelType = "chatml";

    std::string formattedPrompt = model_->formatPrompt(prompt);
    EXPECT_EQ(formattedPrompt, "<|im_start|>user\nHello, how are you?<|im_end|>\n<|im_start|>assistant\n");
}

TEST_F(LLMModelTest, FormatPromptAlpaca) {
    std::string prompt = "Hello, how are you?";
    modelInfo_.modelType = "alpaca";

    std::string formattedPrompt = model_->formatPrompt(prompt);
    EXPECT_EQ(formattedPrompt, "### Instruction:\nHello, how are you?\n\n### Response:\n");
}

TEST_F(LLMModelTest, FormatPromptLlama) {
    std::string prompt = "Hello, how are you?";
    modelInfo_.modelType = "llama";

    std::string formattedPrompt = model_->formatPrompt(prompt);
    EXPECT_EQ(formattedPrompt, "[INST] Hello, how are you? [/INST]");
}

} // namespace testing
} // namespace llm
} // namespace koebridge