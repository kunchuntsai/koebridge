#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <memory>
#include <string>
#include <fstream>
#include <filesystem>
#include "llm/llm_manager.h"
#include "translation/model_manager.h"

namespace fs = std::filesystem;

namespace koebridge {
namespace llm {
namespace testing {

// Mock model manager that returns predefined models
class MockModelManager : public translation::ModelManager {
public:
    explicit MockModelManager(const std::string& modelPath = "")
        : translation::ModelManager(modelPath) {}

    bool initialize() override { return true; }

    bool loadModel(const std::string& modelId) override {
        if (modelId == "valid_model") {
            activeModel_.id = "valid_model";
            activeModel_.path = "_dataset/models/valid_model.bin";
            activeModel_.size = 4096;
            activeModel_.lastModified = std::time(nullptr);
            modelLoaded_ = true;
            return true;
        }
        return false;
    }

    bool unloadModel() override {
        modelLoaded_ = false;
        activeModel_ = translation::ModelInfo{};
        return true;
    }

    std::vector<translation::ModelInfo> getAvailableModels() const override {
        translation::ModelInfo model;
        model.id = "valid_model";
        model.path = "_dataset/models/valid_model.bin";
        model.size = 4096;
        model.lastModified = std::time(nullptr);
        return {model};
    }

    translation::ModelInfo getActiveModel() const override {
        return activeModel_;
    }

    bool isModelLoaded() const override {
        return modelLoaded_;
    }

    bool downloadModel(const std::string& modelId, translation::ProgressCallback callback) override {
        return modelId == "valid_model";
    }

    std::shared_ptr<translation::ITranslationModel> getTranslationModel() override {
        return nullptr;
    }

private:
    translation::ModelInfo activeModel_;
    bool modelLoaded_ = false;
};

class LLMManagerTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Create mock model manager
        modelManager_ = std::make_shared<MockModelManager>();

        // Create LLM manager
        llmManager_ = std::make_unique<LLMManager>(modelManager_);

        // Initialize the manager
        ASSERT_TRUE(llmManager_->initialize());
    }

    void TearDown() override {
        // Clean up
        llmManager_.reset();
        modelManager_.reset();
    }

    std::shared_ptr<MockModelManager> modelManager_;
    std::unique_ptr<LLMManager> llmManager_;
};

TEST_F(LLMManagerTest, Initialization) {
    EXPECT_TRUE(llmManager_->initialize());
}

TEST_F(LLMManagerTest, LoadValidModel) {
    EXPECT_TRUE(llmManager_->loadModel("valid_model"));
    EXPECT_TRUE(llmManager_->isModelLoaded());

    translation::ModelInfo activeModel = llmManager_->getActiveModel();
    EXPECT_EQ(activeModel.id, "valid_model");
}

TEST_F(LLMManagerTest, LoadInvalidModel) {
    EXPECT_FALSE(llmManager_->loadModel("invalid_model"));
    EXPECT_FALSE(llmManager_->isModelLoaded());
}

TEST_F(LLMManagerTest, UnloadModel) {
    // First load a model
    ASSERT_TRUE(llmManager_->loadModel("valid_model"));
    EXPECT_TRUE(llmManager_->isModelLoaded());

    // Now unload it
    EXPECT_TRUE(llmManager_->unloadModel());
    EXPECT_FALSE(llmManager_->isModelLoaded());
}

TEST_F(LLMManagerTest, GetModel) {
    // No model loaded yet
    EXPECT_EQ(llmManager_->getModel(), nullptr);

    // Load a model
    ASSERT_TRUE(llmManager_->loadModel("valid_model"));

    // Now we should have a model
    EXPECT_NE(llmManager_->getModel(), nullptr);
}

TEST_F(LLMManagerTest, TextCompletion) {
    // No model loaded yet
    LLMOutput output = llmManager_->complete("Hello, world!");
    EXPECT_FALSE(output.success);
    EXPECT_TRUE(output.errorMessage.find("No LLM model loaded") != std::string::npos);

    // Load a model
    ASSERT_TRUE(llmManager_->loadModel("valid_model"));

    // Now we should be able to complete text
    output = llmManager_->complete("Hello, world!");
    EXPECT_TRUE(output.success);
    EXPECT_FALSE(output.text.empty());
}

TEST_F(LLMManagerTest, ConfigurationChanges) {
    // Load a model with default config
    ASSERT_TRUE(llmManager_->loadModel("valid_model"));

    // Get initial config
    LLMConfig initialConfig = llmManager_->getConfig();

    // Change the config
    LLMConfig newConfig = initialConfig;
    newConfig.contextSize = 4096;
    newConfig.temperature = 0.9f;

    llmManager_->setConfig(newConfig);

    // Verify the config was changed
    LLMConfig updatedConfig = llmManager_->getConfig();
    EXPECT_EQ(updatedConfig.contextSize, 4096);
    EXPECT_FLOAT_EQ(updatedConfig.temperature, 0.9f);
}

} // namespace testing
} // namespace llm
} // namespace koebridge