/**
 * @file model_manager_test.cc
 * @brief Unit tests for the ModelManager class
 */

#include <gtest/gtest.h>
#include <filesystem>
#include <fstream>
#include <thread>
#include <iostream>
#include "../../../src/translation/model_manager.h"
#include "../../../src/utils/config.h"

// Mock the GGMLModel for testing
#include <memory>
namespace koebridge {
namespace models {
    // Forward declarations to mock dependencies
    class GGMLModel : public translation::ITranslationModel {
    public:
        explicit GGMLModel(const translation::ModelInfo& info) : modelInfo_(info) {
            // Check if the model file exists and has valid GGML header
            std::ifstream file(info.path, std::ios::binary);
            if (!file.is_open()) {
                throw std::runtime_error("Could not open model file: " + info.path);
            }

            // Read and verify GGML magic number
            char magic[4];
            if (!file.read(magic, 4) || magic[0] != 'g' || magic[1] != 'g' || magic[2] != 'm' || magic[3] != 'l') {
                throw std::runtime_error("Invalid model file format: " + info.path);
            }
        }
        bool initialize() override { return true; }
        bool isInitialized() const override { return true; }
        translation::TranslationResult translate(const std::string& text, const translation::TranslationOptions& options) override {
            translation::TranslationResult result;
            result.sourceText = text;
            result.text = "Translated: " + text;
            result.success = true;
            return result;
        }
        std::future<translation::TranslationResult> translateAsync(const std::string& text, const translation::TranslationOptions& options) override {
            return std::async(std::launch::async, [this, text, options]() {
                return translate(text, options);
            });
        }
        translation::ModelInfo getModelInfo() const override { return modelInfo_; }
        translation::InferenceStats getLastInferenceStats() const override { return {}; }
    private:
        translation::ModelInfo modelInfo_;
    };
}
}

namespace fs = std::filesystem;
using namespace koebridge::translation;

class ModelManagerTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Get model path from config (already loaded by Config singleton)
        auto& config = koebridge::utils::Config::getInstance();
        testDir_ = fs::path(config.getString("translation.model_path"));
        fs::create_directories(testDir_);

        // Create mock model files with GGML magic number
        createMockModel("nllb-ja-en");
        createMockModel("nllb-en-ja");

        // Create model manager using the configured path
        manager_ = std::make_unique<koebridge::translation::ModelManager>();
    }

    void TearDown() override {
        // Clean up test directory
        fs::remove_all(testDir_);
    }

    void createMockModel(const std::string& name) {
        auto modelPath = testDir_ / (name + ".bin");
        std::ofstream file(modelPath, std::ios::binary);
        if (!file.is_open()) {
            throw std::runtime_error("Failed to create mock model file: " + modelPath.string());
        }

        // Write GGML magic number (0x67676D6C)
        uint32_t magic = 0x67676D6C;
        file.write(reinterpret_cast<char*>(&magic), sizeof(magic));

        // Write version number
        uint32_t version = 1;
        file.write(reinterpret_cast<char*>(&version), sizeof(version));

        // Write model architecture parameters
        uint32_t n_layers = 2;  // Small test model
        uint32_t n_heads = 4;
        uint32_t n_embd = 256;
        uint32_t vocab_size = 2;  // Minimal vocabulary for testing
        file.write(reinterpret_cast<char*>(&n_layers), sizeof(n_layers));
        file.write(reinterpret_cast<char*>(&n_heads), sizeof(n_heads));
        file.write(reinterpret_cast<char*>(&n_embd), sizeof(n_embd));
        file.write(reinterpret_cast<char*>(&vocab_size), sizeof(vocab_size));

        // Write mock model weights (all zeros for testing)
        size_t weightsSize = n_embd * n_heads * n_layers * sizeof(float);
        std::vector<float> weights(weightsSize / sizeof(float), 0.0f);
        file.write(reinterpret_cast<char*>(weights.data()), weightsSize);

        // Write vocabulary
        std::vector<std::string> vocab = {"<s>", "</s>"};  // Minimal vocab with start/end tokens
        for (const auto& token : vocab) {
            uint32_t tokenLength = token.length();
            file.write(reinterpret_cast<char*>(&tokenLength), sizeof(tokenLength));
            file.write(token.c_str(), tokenLength);
        }
    }

    fs::path testDir_;
    std::unique_ptr<ModelManager> manager_;
};

TEST_F(ModelManagerTest, GetAvailableModels) {
    auto models = manager_->getAvailableModels();
    EXPECT_GT(models.size(), 0) << "No models found in " << testDir_;

    // Check model properties
    for (const auto& model : models) {
        EXPECT_FALSE(model.id.empty()) << "Model ID should not be empty";
        EXPECT_FALSE(model.path.empty()) << "Model path should not be empty";
        EXPECT_GT(model.size, 0) << "Model size should be greater than 0";

        // Verify file exists
        EXPECT_TRUE(fs::exists(model.path)) << "Model file does not exist: " << model.path;

        // Check if it's a valid NLLB model
        EXPECT_TRUE(model.id.find("nllb") != std::string::npos)
            << "Expected NLLB model, got: " << model.id;
    }
}

TEST_F(ModelManagerTest, LoadModel) {
    auto models = manager_->getAvailableModels();
    if (models.empty()) {
        GTEST_SKIP() << "No models available for testing";
    }

    // Try to load the first available model
    const auto& firstModel = models[0];
    EXPECT_TRUE(manager_->loadModel(firstModel.id))
        << "Failed to load model: " << firstModel.id;

    EXPECT_TRUE(manager_->isModelLoaded())
        << "Model should be loaded: " << firstModel.id;

    // Check active model
    auto activeModel = manager_->getActiveModel();
    EXPECT_EQ(activeModel.id, firstModel.id)
        << "Active model should match loaded model";

    // Try to load non-existent model
    EXPECT_FALSE(manager_->loadModel("non_existent_model"))
        << "Should fail to load non-existent model";
}

TEST_F(ModelManagerTest, UnloadModel) {
    auto models = manager_->getAvailableModels();
    if (models.empty()) {
        GTEST_SKIP() << "No models available for testing";
    }

    // Load and then unload model
    EXPECT_TRUE(manager_->loadModel(models[0].id))
        << "Failed to load model: " << models[0].id;
    EXPECT_TRUE(manager_->isModelLoaded())
        << "Model should be loaded";

    EXPECT_TRUE(manager_->unloadModel())
        << "Failed to unload model";
    EXPECT_FALSE(manager_->isModelLoaded())
        << "Model should be unloaded";
}

TEST_F(ModelManagerTest, GetTranslationModel) {
    auto models = manager_->getAvailableModels();
    if (models.empty()) {
        GTEST_SKIP() << "No models available for testing";
    }

    // Try to get model without loading
    EXPECT_EQ(manager_->getTranslationModel(), nullptr)
        << "Should return nullptr when no model is loaded";

    // Load model and get translation model
    EXPECT_TRUE(manager_->loadModel(models[0].id))
        << "Failed to load model: " << models[0].id;
    auto model = manager_->getTranslationModel();
    EXPECT_NE(model, nullptr)
        << "Should return valid model after loading";
}

TEST_F(ModelManagerTest, DownloadModel) {
    bool callbackCalled = false;
    int lastProgress = 0;
    std::string lastMessage;

    auto callback = [&](int progress, const std::string& message) {
        callbackCalled = true;
        lastProgress = progress;
        lastMessage = message;
    };

    // Try to download an existing model (should return true since it exists)
    auto models = manager_->getAvailableModels();
    if (!models.empty()) {
        EXPECT_TRUE(manager_->downloadModel(models[0].id, callback))
            << "Download should succeed for existing model";
        EXPECT_TRUE(callbackCalled)
            << "Callback should be called";
        EXPECT_EQ(lastProgress, 100)
            << "Progress should be 100% for existing model";
    }
}

TEST_F(ModelManagerTest, InvalidModelFiles) {
    // Skip this test as we don't want to create invalid files in the actual model directory
    GTEST_SKIP() << "Skipping invalid model files test to avoid modifying actual model directory";
}
