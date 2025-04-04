#include <gtest/gtest.h>
#include "../../../src/translation/model_manager.h"
#include "../../../include/interfaces/i_model_manager.h"

class ModelManagerTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Setup test environment
    }

    void TearDown() override {
        // Cleanup test environment
    }
};

TEST_F(ModelManagerTest, TestModelDiscovery) {
    ModelManager manager("./test_models");
    auto models = manager.getAvailableModels();
    // Verify model discovery works correctly
}

TEST_F(ModelManagerTest, TestModelLoading) {
    ModelManager manager("./test_models");
    bool success = manager.loadModel("test_model");
    EXPECT_TRUE(success);
    EXPECT_TRUE(manager.isModelLoaded());
}

TEST_F(ModelManagerTest, TestModelSwitching) {
    ModelManager manager("./test_models");
    manager.loadModel("model1");
    auto model1 = manager.getActiveModel();
    
    manager.loadModel("model2");
    auto model2 = manager.getActiveModel();
    
    EXPECT_NE(model1.id, model2.id);
}

TEST_F(ModelManagerTest, TestErrorHandling) {
    ModelManager manager("./test_models");
    bool result = manager.loadModel("non_existent_model");
    EXPECT_FALSE(result);
}
