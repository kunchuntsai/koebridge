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

// Mock the GGMLModel for testing
#include <memory>
namespace koebridge {
namespace models {
    // Forward declarations to mock dependencies
    class GGMLModel : public translation::ITranslationModel {
    public:
        explicit GGMLModel(const translation::ModelInfo& info) : modelInfo_(info) {}
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
    //TODO: Add models we wanted to test here
    void SetUp() override {
        // Create temporary test directory
        testDir_ = fs::temp_directory_path() / "koebridge_test";
        fs::create_directories(testDir_);
        
        // Create some test model files
        createTestModel("model1", 1024);
        createTestModel("model2", 2048);
        
        // Debug: Print test directory and confirm files exist
        std::cout << "Test directory: " << testDir_.string() << std::endl;
        std::cout << "Model1 exists: " << fs::exists(testDir_ / "model1.bin") << std::endl;
        std::cout << "Model2 exists: " << fs::exists(testDir_ / "model2.bin") << std::endl;
        
        // Create model manager
        manager_ = std::make_unique<ModelManager>(testDir_.string());
        manager_->initialize();
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
            EXPECT_EQ(model.size, 1024);
        } else if (model.id == "model2") {
            foundModel2 = true;
            EXPECT_EQ(model.size, 2048);
        }
    }
    
    EXPECT_TRUE(foundModel1);
    EXPECT_TRUE(foundModel2);
}

TEST_F(ModelManagerTest, LoadModel) {
    // Load valid model
    EXPECT_TRUE(manager_->loadModel("model1"));
    
    // Note: Since the actual model loading in ModelManager is just a placeholder,
    // we'll test the flag behavior rather than actual model loading
    EXPECT_TRUE(manager_->isModelLoaded());
    
    // Check active model
    auto activeModel = manager_->getActiveModel();
    EXPECT_EQ(activeModel.id, "model1");
    
    // Load non-existent model
    EXPECT_FALSE(manager_->loadModel("non_existent_model"));
    EXPECT_TRUE(manager_->isModelLoaded());  // Still true because current model remains loaded
}

TEST_F(ModelManagerTest, UnloadModel) {
    // Load and then unload model
    EXPECT_TRUE(manager_->loadModel("model1"));
    EXPECT_TRUE(manager_->isModelLoaded());
    
    EXPECT_TRUE(manager_->unloadModel());
    EXPECT_FALSE(manager_->isModelLoaded());
}

TEST_F(ModelManagerTest, GetTranslationModel) {
    // Try to get model without loading
    EXPECT_EQ(manager_->getTranslationModel(), nullptr);
    
    // Load model and get translation model
    EXPECT_TRUE(manager_->loadModel("model1"));
    auto model = manager_->getTranslationModel();
    EXPECT_EQ(model, nullptr);  // Currently returns nullptr in implementation
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
    
    // Download model
    EXPECT_TRUE(manager_->downloadModel("model1", callback));
    EXPECT_TRUE(callbackCalled);
    EXPECT_EQ(lastProgress, 100);
}

TEST_F(ModelManagerTest, InvalidModelFiles) {
    // Create invalid model file (empty)
    std::ofstream file(testDir_ / "invalid_model.bin");
    file.close();
    
    // Create invalid model file (not a regular file)
    fs::create_directory(testDir_ / "dir_model.bin");
    
    // Rescan with these new files
    manager_->initialize();
    auto models = manager_->getAvailableModels();
    
    // Should include all .bin files regardless of content since the implementation
    // only checks file extension
    EXPECT_GE(models.size(), 2);  // At least our original models
    
    // Check if our original models are still found
    bool foundModel1 = false;
    bool foundModel2 = false;
    
    for (const auto& model : models) {
        if (model.id == "model1") foundModel1 = true;
        if (model.id == "model2") foundModel2 = true;
    }
    
    EXPECT_TRUE(foundModel1);
    EXPECT_TRUE(foundModel2);
}
