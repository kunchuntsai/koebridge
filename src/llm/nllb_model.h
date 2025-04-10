/**
 * @file nllb_model.h
 * @brief Definition of the NLLB model
 */

#pragma once

#include "llm_model.h"
#include <string>
#include <vector>
#include <memory>
#include <future>
#include <unordered_map>

namespace koebridge {
namespace llm {

/**
 * @class NLLBModel
 * @brief Implementation of the NLLB model for translation
 */
class NLLBModel : public LLMModel {
public:
    friend class koebridge::llm::testing::NLLBModelTest;

    /**
     * @brief Constructor for NLLBModel
     * @param modelInfo Information about the model to load
     * @param sourceLanguage Source language code
     * @param targetLanguage Target language code
     * @param config Configuration options for the LLM
     */
    NLLBModel(const translation::ModelInfo& modelInfo,
              const std::string& sourceLanguage,
              const std::string& targetLanguage,
              const LLMConfig& config = LLMConfig());

    /**
     * @brief Destructor for NLLBModel
     */
    ~NLLBModel() override;

    /**
     * @brief Initialize the NLLB model
     * @return bool True if initialization was successful
     */
    bool initialize() override;

    /**
     * @brief Translate text from source to target language
     * @param text Text to translate
     * @return LLMOutput The translation result
     */
    LLMOutput translate(const std::string& text);

    /**
     * @brief Set the source language
     * @param language Language code
     */
    void setSourceLanguage(const std::string& language);

    /**
     * @brief Set the target language
     * @param language Language code
     */
    void setTargetLanguage(const std::string& language);

    /**
     * @brief Get the source language
     * @return std::string Source language code
     */
    std::string getSourceLanguage() const;

    /**
     * @brief Get the target language
     * @return std::string Target language code
     */
    std::string getTargetLanguage() const;

    /**
     * @brief Format the input prompt according to the model's requirements
     * @param prompt The input prompt to format
     * @return std::string The formatted prompt
     */
    std::string formatPrompt(const std::string& prompt) override;

    /**
     * @brief Convert text to token IDs using the local tokenizer
     * @param text Input text to tokenize
     * @return std::vector<int> Vector of token IDs
     */
    std::vector<int> localTokenize(const std::string& text) override;

    /**
     * @brief Convert token IDs back to text using the local tokenizer
     * @param tokens Vector of token IDs to detokenize
     * @return std::string The detokenized text
     */
    std::string localDetokenize(const std::vector<int>& tokens) override;

protected:
    /**
     * @brief Initialize language-specific tokens
     */
    void initializeLanguageTokens();

private:
    std::string sourceLanguage_;                   ///< Source language code
    std::string targetLanguage_;                   ///< Target language code
    std::unordered_map<std::string, int> specialTokens_; ///< Special tokens for languages
};

} // namespace llm
} // namespace koebridge