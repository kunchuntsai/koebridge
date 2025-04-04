#include <gtest/gtest.h>
#include <memory>
#include "../../../src/translation/translator.h"
#include "../../../src/translation/model_manager.h"
#include "../../../src/stt/whisper_wrapper.h"

class EndToEndTranslationTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Setup test environment
        modelManager = std::make_shared<ModelManager>("./test_models");
        translator = std::make_shared<TranslationService>(modelManager);
        translator->initialize();
    }

    void TearDown() override {
        translator->shutdown();
    }

    std::shared_ptr<ModelManager> modelManager;
    std::shared_ptr<TranslationService> translator;
};

TEST_F(EndToEndTranslationTest, TestSimpleTranslation) {
    // Load a test model
    modelManager->loadModel("small_test_model");
    
    // Translate a simple phrase
    std::string japaneseText = "こんにちは、世界！";
    std::string translatedText = translator->translateText(japaneseText);
    
    // Verify translation result
    EXPECT_FALSE(translatedText.empty());
    // In a real test, you might check for specific contents
    // EXPECT_EQ(translatedText, "Hello, world!");
}

TEST_F(EndToEndTranslationTest, TestAsyncTranslation) {
    // Load a test model
    modelManager->loadModel("small_test_model");
    
    // Translate asynchronously
    std::string japaneseText = "さようなら";
    bool callbackInvoked = false;
    
    translator->translateTextAsync(japaneseText, [&callbackInvoked](const TranslationResult& result) {
        callbackInvoked = true;
        // Verify result
        EXPECT_TRUE(result.success);
        EXPECT_FALSE(result.translatedText.empty());
    });
    
    // Wait for callback (simplified for example)
    // In a real test, you'd use proper synchronization
    std::this_thread::sleep_for(std::chrono::seconds(2));
    EXPECT_TRUE(callbackInvoked);
}
