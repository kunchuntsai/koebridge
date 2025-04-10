#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <memory>
#include <string>
#include <fstream>
#include <filesystem>
#include "llm/nllb_model.h"
#include "translation/data_structures.h"

namespace fs = std::filesystem;

namespace koebridge {
namespace llm {
namespace testing {

class NLLBModelTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Set up model info
        modelInfo_.id = "test_nllb";
        modelInfo_.path = "/path/to/model";
        modelInfo_.size = 4096;
        modelInfo_.lastModified = std::time(nullptr);
        modelInfo_.modelType = "nllb";

        // Create default config
        config_.contextSize = 2048;
        config_.temperature = 0.7f;
        config_.maxLength = 1024;
        config_.beamSize = 4;

        // Create model instance
        model_ = std::make_unique<NLLBModel>(modelInfo_, "eng", "jpn", config_);
    }

    void TearDown() override {
        model_.reset();
    }

    translation::ModelInfo modelInfo_;
    LLMConfig config_;
    std::unique_ptr<NLLBModel> model_;
};

TEST_F(NLLBModelTest, Initialize) {
    ASSERT_TRUE(model_->initialize());
}

TEST_F(NLLBModelTest, SetLanguages) {
    model_->setSourceLanguage("fra");
    model_->setTargetLanguage("deu");

    EXPECT_EQ(model_->getSourceLanguage(), "fra");
    EXPECT_EQ(model_->getTargetLanguage(), "deu");
}

TEST_F(NLLBModelTest, Translate) {
    ASSERT_TRUE(model_->initialize());

    std::string text = "Hello, how are you?";
    LLMOutput output = model_->translate(text);

    EXPECT_TRUE(output.success);
    EXPECT_FALSE(output.text.empty());
}

TEST_F(NLLBModelTest, PromptFormatting) {
    // Initialize the model
    ASSERT_TRUE(model_->initialize());

    // Test prompt formatting
    std::string prompt = "こんにちは";
    std::string formattedPrompt = model_->formatPrompt(prompt);

    // The formatted prompt should contain language tokens
    EXPECT_TRUE(formattedPrompt.find("250025") != std::string::npos); // Japanese token
    EXPECT_TRUE(formattedPrompt.find("250004") != std::string::npos); // English token
    EXPECT_TRUE(formattedPrompt.find(prompt) != std::string::npos);   // Original text
}

TEST_F(NLLBModelTest, UnsupportedLanguagePair) {
    // Initialize the model
    ASSERT_TRUE(model_->initialize());

    // Set an unsupported language pair
    model_->setSourceLanguage("invalid_lang");
    model_->setTargetLanguage("also_invalid");

    // Test translation with unsupported languages
    LLMOutput output = model_->translate("Hello");

    // The translation should still work, but with a warning
    EXPECT_TRUE(output.success);
    EXPECT_FALSE(output.text.empty());
}

} // namespace testing
} // namespace llm
} // namespace koebridge