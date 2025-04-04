/**
 * @file model_manager_test.cc
 * @brief Unit tests for the ModelManager class
 */

#include <gtest/gtest.h>
#include <filesystem>
#include <fstream>
#include <thread>
#include "../../../src/translation/model_manager.h"

namespace fs = std::filesystem;

class ModelManagerTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Create temporary test directory
        testDir_ = fs::temp_directory_path() / "koebridge_test";
        fs::create_directories(testDir_);
        
        // Create some test model files
        createTestModel("model1", 1024);
        createTestModel("model2", 2048);
        
        // Create model manager
        manager_ = std::make_unique<ModelManager>(testDir_.string());
    }
    
    void TearDown() override {
        // Clean up test directory
        fs::remove_all(testDir_);
    }
    
    void createTestModel(const std::string& name, size_t size) {
        std::ofstream file(testDir_ / (name + ".bin"), std::ios::binary);
        std::vector<char> data(size, 0);
        file.write(data.data(), data.size());
        file.close();
    }
    
    fs::path testDir_;
    std::unique_ptr<ModelManager> manager_;
};

TEST_F(ModelManagerTest, GetAvailableModels) {
    auto models = manager_->getAvailableModels();
    EXPECT_EQ(models.size(), 2);
    
    // Check model properties
    bool foundModel1 = false;
    bool foundModel2 = false;
    
    for (const auto& model : models) {
        if (model.id == "model1") {
            foundModel1 = true;
            EXPECT_EQ(model.sizeBytes, 1024);
        } else if (model.id == "model2") {
            foundModel2 = true;
            EXPECT_EQ(model.sizeBytes, 2048);
        }
    }
    
    EXPECT_TRUE(foundModel1);
    EXPECT_TRUE(foundModel2);
}

TEST_F(ModelManagerTest, LoadModel) {
    // Load valid model
    EXPECT_TRUE(manager_->loadModel("model1"));
    EXPECT_TRUE(manager_->isModelLoaded());
    
    // Check active model
    auto activeModel = manager_->getActiveModel();
    EXPECT_EQ(activeModel.id, "model1");
    
    // Load non-existent model
    EXPECT_FALSE(manager_->loadModel("non_existent_model"));
    EXPECT_FALSE(manager_->isModelLoaded());
}

TEST_F(ModelManagerTest, UnloadModel) {
    // Load and then unload model
    EXPECT_TRUE(manager_->loadModel("model1"));
    EXPECT_TRUE(manager_->isModelLoaded());
    
    manager_->unloadCurrentModel();
    EXPECT_FALSE(manager_->isModelLoaded());
}

TEST_F(ModelManagerTest, GetTranslationModel) {
    // Try to get model without loading
    EXPECT_EQ(manager_->getTranslationModel(), nullptr);
    
    // Load model and get translation model
    EXPECT_TRUE(manager_->loadModel("model1"));
    auto model = manager_->getTranslationModel();
    EXPECT_NE(model, nullptr);
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
    
    // Download existing model
    EXPECT_TRUE(manager_->downloadModel("model1", callback));
    EXPECT_TRUE(callbackCalled);
    EXPECT_EQ(lastProgress, 100);
    EXPECT_EQ(lastMessage, "Model already downloaded");
    
    // Download non-existent model
    callbackCalled = false;
    EXPECT_FALSE(manager_->downloadModel("non_existent_model", callback));
    EXPECT_FALSE(callbackCalled);
}

TEST_F(ModelManagerTest, InvalidModelFiles) {
    // Create invalid model file (empty)
    std::ofstream file(testDir_ / "invalid_model.bin");
    file.close();
    
    // Create invalid model file (not a regular file)
    fs::create_directory(testDir_ / "dir_model.bin");
    
    // Create new manager to scan for models
    auto manager = std::make_unique<ModelManager>(testDir_.string());
    auto models = manager->getAvailableModels();
    
    // Should only find valid models
    EXPECT_EQ(models.size(), 2);
    for (const auto& model : models) {
        EXPECT_NE(model.id, "invalid_model");
        EXPECT_NE(model.id, "dir_model");
    }
}
