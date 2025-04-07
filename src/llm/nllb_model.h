/**
 * @file nllb_model.h
 * @brief Definition of the NLLB model
 */

#pragma once

#include "llm/llm_model.h"
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
    /**
     * @brief Constructor
     * @param modelInfo Information about the model
     * @param sourceLanguage Source language code
     * @param targetLanguage Target language code
     * @param config LLM configuration
     */
    NLLBModel(
        const translation::ModelInfo& modelInfo,
        const std::string& sourceLanguage,
        const std::string& targetLanguage,
        const LLMConfig& config
    );
    
    /**
     * @brief Destructor
     */
    ~NLLBModel() override;
    
    /**
     * @brief Initialize the model
     * @return True if initialization was successful, false otherwise
     */
    bool initialize() override;
    
    /**
     * @brief Translate text from source to target language
     * @param text Text to translate
     * @return Translation result
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
     * @return Source language code
     */
    std::string getSourceLanguage() const;
    
    /**
     * @brief Get the target language
     * @return Target language code
     */
    std::string getTargetLanguage() const;
    
protected:
    /**
     * @brief Format a prompt for the model
     * @param prompt Raw input prompt
     * @return Formatted prompt
     */
    std::string formatPrompt(const std::string& prompt);
    
    /**
     * @brief Convert input text to token IDs
     * @param text Input text
     * @return Vector of token IDs
     */
    std::vector<int> localTokenize(const std::string& text);
    
    /**
     * @brief Convert token IDs back to text
     * @param tokens Vector of token IDs
     * @return Output text
     */
    std::string localDetokenize(const std::vector<int>& tokens);
    
private:
    /**
     * @brief Initialize language-specific tokens
     */
    void initializeLanguageTokens();
    
    std::string sourceLanguage_;                   ///< Source language code
    std::string targetLanguage_;                   ///< Target language code
    std::unordered_map<std::string, int> specialTokens_; ///< Special tokens for languages
};

} // namespace llm
} // namespace koebridge 