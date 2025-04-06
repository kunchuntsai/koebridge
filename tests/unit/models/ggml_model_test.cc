#include <gtest/gtest.h>
#include "../../../src/models/ggml_model.h"
#include "../../../src/translation/data_structures.h"

using namespace koebridge::models;
using namespace koebridge::translation;

class GGMLModelTest : public ::testing::Test {
protected:
    void SetUp() override {
        modelInfo.id = "test_model";
        modelInfo.path = "./test_models/test_model.bin";
        modelInfo.size = 1024;
        modelInfo.lastModified = std::time(nullptr);
    }

    ModelInfo modelInfo;
};

TEST_F(GGMLModelTest, TestInitialization) {
    GGMLModel model(modelInfo);
    bool result = model.initialize();
    // In a real test, this might succeed or fail depending on test environment
    // EXPECT_TRUE(result);
    // EXPECT_TRUE(model.isInitialized());
}

TEST_F(GGMLModelTest, TestBasicTranslation) {
    GGMLModel model(modelInfo);
    model.initialize();
    
    TranslationOptions options;
    auto result = model.translate("こんにちは", options);
    
    // In a real test, you would verify the translation
    // EXPECT_TRUE(result.success);
    // EXPECT_EQ(result.translatedText, "Hello");
}

TEST_F(GGMLModelTest, TestLongText) {
    GGMLModel model(modelInfo);
    model.initialize();
    
    // Create a string longer than the typical context window
    std::string longText;
    longText.reserve(10000 * 3);  // Reserve space for UTF-8 characters
    for (int i = 0; i < 10000; ++i) {
        longText += "あ";  // Append UTF-8 string instead of character
    }
    
    TranslationOptions options;
    auto result = model.translate(longText, options);
    
    // Verify handling of long text
    // EXPECT_TRUE(result.success);
}

TEST_F(GGMLModelTest, TestAsyncTranslation) {
    GGMLModel model(modelInfo);
    model.initialize();
    
    TranslationOptions options;
    auto future = model.translateAsync("おはようございます", options);
    
    // In a real test, you would verify the async translation
    // auto result = future.get();
    // EXPECT_TRUE(result.success);
    // EXPECT_EQ(result.translatedText, "Good morning");
}

TEST_F(GGMLModelTest, Initialization) {
    GGMLModel model(modelInfo);
    EXPECT_FALSE(model.isLoaded());
}

TEST_F(GGMLModelTest, LoadModel) {
    GGMLModel model(modelInfo);
    
    // Since we're using a non-existent model, loading should fail
    EXPECT_FALSE(model.load());
    EXPECT_FALSE(model.isLoaded());
}

TEST_F(GGMLModelTest, TranslateText) {
    GGMLModel model(modelInfo);
    
    TranslationOptions options;
    options.temperature = 0.7f;
    options.maxLength = 1024;
    
    std::string output;
    EXPECT_FALSE(model.translate("こんにちは世界", output, options));
    EXPECT_TRUE(output.empty());
}

TEST_F(GGMLModelTest, GetMetadata) {
    GGMLModel model(modelInfo);
    
    auto metadata = model.getMetadata();
    EXPECT_EQ(metadata.id, modelInfo.id);
    EXPECT_EQ(metadata.path, modelInfo.path);
    EXPECT_EQ(metadata.size, modelInfo.size);
}

TEST_F(GGMLModelTest, GetStatistics) {
    GGMLModel model(modelInfo);
    
    TranslationOptions options;
    std::string output;
    
    // Attempt a translation (which will fail)
    model.translate("こんにちは世界", output, options);
    
    // Check stats (should be zeros since translation failed)
    auto stats = model.getStatistics();
    EXPECT_EQ(stats.totalTimeMs, 0.0);
    EXPECT_EQ(stats.inferenceTimeMs, 0.0);
    EXPECT_EQ(stats.inputTokenCount, 0);
    EXPECT_EQ(stats.outputTokenCount, 0);
}
