/**
 * @file nllb_model.cc
 * @brief Implementation of the NLLB model
 */

#include "llm/nllb_model.h"
#include "utils/config.h"
#include "utils/logger.h"
#include <iostream>
#include <future>
#include <algorithm>

namespace koebridge {
namespace llm {

NLLBModel::NLLBModel(
    const translation::ModelInfo& modelInfo,
    const std::string& sourceLanguage,
    const std::string& targetLanguage,
    const LLMConfig& config
) : LLMModel(modelInfo, config),
    sourceLanguage_(sourceLanguage),
    targetLanguage_(targetLanguage) {
    // Additional initialization will be done in initialize()
}

NLLBModel::~NLLBModel() {
    // Cleanup will be handled by the base class
}

bool NLLBModel::initialize() {
    // First call the base class implementation
    if (!LLMModel::initialize()) {
        std::cerr << "Failed to initialize base LLM model" << std::endl;
        return false;
    }

    // Initialize language-specific tokens
    initializeLanguageTokens();

    std::cout << "NLLB model initialized for " << sourceLanguage_ << " -> " << targetLanguage_ << std::endl;
    return true;
}

LLMOutput NLLBModel::translate(const std::string& text) {
    // Use the complete method from the parent class
    return complete(text);
}

void NLLBModel::setSourceLanguage(const std::string& language) {
    sourceLanguage_ = language;
    // Re-initialize language tokens when language changes
    initializeLanguageTokens();
}

void NLLBModel::setTargetLanguage(const std::string& language) {
    targetLanguage_ = language;
    // Re-initialize language tokens when language changes
    initializeLanguageTokens();
}

std::string NLLBModel::getSourceLanguage() const {
    return sourceLanguage_;
}

std::string NLLBModel::getTargetLanguage() const {
    return targetLanguage_;
}

std::string NLLBModel::formatPrompt(const std::string& prompt) {
    // Get source and target language tokens
    auto sourceToken = specialTokens_.find("__" + sourceLanguage_ + "__");
    auto targetToken = specialTokens_.find("__" + targetLanguage_ + "__");

    if (sourceToken == specialTokens_.end() || targetToken == specialTokens_.end()) {
        std::cerr << "Unsupported language pair: " << sourceLanguage_ << " -> " << targetLanguage_ << std::endl;
        return prompt;
    }

    // Format: <source_lang> text <target_lang>
    return std::to_string(sourceToken->second) + " " + prompt + " " + std::to_string(targetToken->second);
}

std::vector<int> NLLBModel::localTokenize(const std::string& text) {
    std::vector<int> tokens;

    // Add BOS token
    tokens.push_back(1);  // BOS token ID

    // Get the tokenizer from the engine
    auto* tokenizer = static_cast<sentencepiece::SentencePieceProcessor*>(engine_->getTokenizer());
    if (!tokenizer) {
        std::cerr << "Tokenizer not available" << std::endl;
        return tokens;
    }

    // Add source language token
    auto sourceIt = specialTokens_.find(sourceLanguage_);
    if (sourceIt != specialTokens_.end()) {
        tokens.push_back(sourceIt->second);
    }

    // Tokenize the text
    std::vector<int> textTokens;
    tokenizer->Encode(text, &textTokens);
    tokens.insert(tokens.end(), textTokens.begin(), textTokens.end());

    // Add target language token
    auto targetIt = specialTokens_.find(targetLanguage_);
    if (targetIt != specialTokens_.end()) {
        tokens.push_back(targetIt->second);
    }

    // Add EOS token
    tokens.push_back(2);  // EOS token ID

    return tokens;
}

std::string NLLBModel::localDetokenize(const std::vector<int>& tokens) {
    // Get the tokenizer from the engine
    auto* tokenizer = static_cast<sentencepiece::SentencePieceProcessor*>(engine_->getTokenizer());
    if (!tokenizer) {
        std::cerr << "Tokenizer not available" << std::endl;
        return "";
    }

    // Remove special tokens (BOS, EOS, and language tokens)
    std::vector<int> textTokens;
    if (tokens.size() > 4) {  // BOS + src_lang + text + tgt_lang + EOS
        textTokens.assign(tokens.begin() + 2, tokens.end() - 2);
    }

    // Detokenize
    std::string text;
    tokenizer->Decode(textTokens, &text);
    return text;
}

void NLLBModel::initializeLanguageTokens() {
    // Initialize special tokens for NLLB model
    specialTokens_ = {
        {"__ja__", 250025},  // Japanese
        {"__en__", 250004},  // English
        {"__zh__", 250010},  // Chinese
        {"__ko__", 250011},  // Korean
        {"__fr__", 250005},  // French
        {"__de__", 250006},  // German
        {"__es__", 250007},  // Spanish
        {"__it__", 250008},  // Italian
        {"__pt__", 250009},  // Portuguese
        {"__ru__", 250012}   // Russian
    };
}

} // namespace llm
} // namespace koebridge