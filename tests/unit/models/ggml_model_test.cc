#include <gtest/gtest.h>
#include "../../../src/models/ggml_model.h"
#include "../../../src/translation/data_structures.h"
#include <fstream>
#include <filesystem>

using namespace koebridge::models;
using namespace koebridge::translation;

class GGMLModelTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Create test model directory
        std::filesystem::create_directories("./test_models");
        
        // Create a dummy GGML model file for testing
        createDummyModelFile();
        
        modelInfo.id = "test_model";
        modelInfo.path = "./test_models/test_model.bin";
        modelInfo.size = 1024;
        modelInfo.lastModified = std::time(nullptr);
        modelInfo.capabilities = {"translation"};
        modelInfo.modelType = "nllb";
        modelInfo.description = "Test NLLB model";
        modelInfo.sourceLanguage = "jpn_Jpan";
        modelInfo.targetLanguage = "eng_Latn";
    }
    
    void TearDown() override {
        // Clean up test files
        std::filesystem::remove_all("./test_models");
    }
    
    void createDummyModelFile() {
        std::ofstream file("./test_models/test_model.bin", std::ios::binary);
        
        // Write GGML magic number
        uint32_t magic = 0x67676D6C;  // "ggml"
        file.write(reinterpret_cast<char*>(&magic), sizeof(magic));
        
        // Write version
        uint32_t version = 1;
        file.write(reinterpret_cast<char*>(&version), sizeof(version));
        
        // Write model parameters
        uint32_t n_layers = 2;
        uint32_t n_heads = 4;
        uint32_t n_embd = 64;
        uint32_t vocab_size = 256;
        
        file.write(reinterpret_cast<char*>(&n_layers), sizeof(n_layers));
        file.write(reinterpret_cast<char*>(&n_heads), sizeof(n_heads));
        file.write(reinterpret_cast<char*>(&n_embd), sizeof(n_embd));
        file.write(reinterpret_cast<char*>(&vocab_size), sizeof(vocab_size));
        
        // Write dummy weights
        size_t weightsSize = n_layers * n_heads * n_embd * sizeof(float);
        std::vector<float> weights(weightsSize / sizeof(float), 0.1f);
        file.write(reinterpret_cast<char*>(weights.data()), weightsSize);
        
        // Write vocabulary
        for (uint32_t i = 0; i < vocab_size; ++i) {
            std::string token = std::to_string(i);
            uint32_t tokenLength = token.length();
            file.write(reinterpret_cast<char*>(&tokenLength), sizeof(tokenLength));
            file.write(token.c_str(), tokenLength);
        }
    }

    ModelInfo modelInfo;
};

TEST_F(GGMLModelTest, TestInitialization) {
    GGMLModel model(modelInfo);
    EXPECT_TRUE(model.initialize());
    EXPECT_TRUE(model.isInitialized());
}

TEST_F(GGMLModelTest, TestModelInfo) {
    GGMLModel model(modelInfo);
    auto info = model.getModelInfo();
    EXPECT_EQ(info.id, modelInfo.id);
    EXPECT_EQ(info.path, modelInfo.path);
    EXPECT_EQ(info.size, modelInfo.size);
    EXPECT_EQ(info.modelType, modelInfo.modelType);
    EXPECT_EQ(info.sourceLanguage, modelInfo.sourceLanguage);
    EXPECT_EQ(info.targetLanguage, modelInfo.targetLanguage);
}

TEST_F(GGMLModelTest, TestBasicTranslation) {
    GGMLModel model(modelInfo);
    ASSERT_TRUE(model.initialize());
    
    TranslationOptions options;
    auto result = model.translate("こんにちは", options);
    
    EXPECT_TRUE(result.success);
    EXPECT_FALSE(result.text.empty());
    EXPECT_GT(result.metrics.totalTimeMs, 0);
    EXPECT_GT(result.metrics.inferenceTimeMs, 0);
    EXPECT_GT(result.metrics.inputTokenCount, 0);
}

TEST_F(GGMLModelTest, TestAsyncTranslation) {
    GGMLModel model(modelInfo);
    ASSERT_TRUE(model.initialize());
    
    TranslationOptions options;
    auto future = model.translateAsync("おはようございます", options);
    
    auto result = future.get();
    EXPECT_TRUE(result.success);
    EXPECT_FALSE(result.text.empty());
    EXPECT_GT(result.metrics.totalTimeMs, 0);
}

TEST_F(GGMLModelTest, TestInvalidModel) {
    ModelInfo invalidInfo = modelInfo;
    invalidInfo.path = "./test_models/nonexistent.bin";
    
    GGMLModel model(invalidInfo);
    EXPECT_FALSE(model.initialize());
    EXPECT_FALSE(model.isInitialized());
}

TEST_F(GGMLModelTest, TestInferenceStats) {
    GGMLModel model(modelInfo);
    ASSERT_TRUE(model.initialize());
    
    auto stats = model.getLastInferenceStats();
    EXPECT_GE(stats.inferenceTimeMs, 0);
    EXPECT_GE(stats.tokensProcessed, 0);
    EXPECT_GE(stats.memoryUsed, 0);
}

TEST_F(GGMLModelTest, TestTranslationOptions) {
    GGMLModel model(modelInfo);
    ASSERT_TRUE(model.initialize());
    
    TranslationOptions options;
    options.temperature = 0.7f;
    options.maxLength = 1024;
    options.beamSize = 4;
    options.style = TranslationOptions::Style::NATURAL;
    
    auto result = model.translate("こんにちは", options);
    EXPECT_TRUE(result.success);
    EXPECT_FALSE(result.text.empty());
}

TEST_F(GGMLModelTest, TestEmptyInput) {
    GGMLModel model(modelInfo);
    ASSERT_TRUE(model.initialize());
    
    TranslationOptions options;
    auto result = model.translate("", options);
    EXPECT_FALSE(result.success);
    EXPECT_FALSE(result.errorMessage.empty());
}

TEST_F(GGMLModelTest, TestLongInput) {
    GGMLModel model(modelInfo);
    ASSERT_TRUE(model.initialize());
    
    // Create a long input string
    std::string longText;
    longText.reserve(10000 * 3);  // Reserve space for UTF-8 characters
    for (int i = 0; i < 10000; ++i) {
        longText += "あ";
    }
    
    TranslationOptions options;
    auto result = model.translate(longText, options);
    EXPECT_TRUE(result.success);
    EXPECT_FALSE(result.text.empty());
    EXPECT_GT(result.metrics.inputTokenCount, 10000);
}
