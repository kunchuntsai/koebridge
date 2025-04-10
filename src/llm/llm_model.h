/**
 * @file llm_model.h
 * @brief Header file for the LLM model implementation
 */

#pragma once

#include <string>
#include <memory>
#include <future>
#include <vector>
#include "models/ggml_model.h"
#include "inference/engine.h"
#include "translation/data_structures.h"
#include "utils/config.h"
#include <sentencepiece_processor.h>
#include <gtest/gtest.h>

namespace koebridge {
namespace llm {

// Forward declarations for test classes
namespace testing {
class LLMModelTest;
class NLLBModelTest;
}

/**
 * @struct LLMConfig
 * @brief Configuration options for LLM models
 */
struct LLMConfig {
    int contextSize = 2048;            ///< Maximum context size
    float temperature = 0.7f;          ///< Sampling temperature
    int maxLength = 1024;              ///< Maximum generation length
    int beamSize = 4;                  ///< Beam search size
    int numThreads = 4;                ///< Number of threads for inference
    float topP = 0.9f;                 ///< Top-p sampling threshold
    int topK = 40;                     ///< Top-k sampling threshold
    float repeatPenalty = 1.1f;        ///< Penalty for repeated tokens
    bool useGPU = false;              ///< Whether to use GPU acceleration
    std::string deviceType = "cpu";   ///< Device type (cpu, metal, vulkan, cuda)
};

/**
 * @struct LLMOutput
 * @brief Output from LLM generation
 */
struct LLMOutput {
    std::string text;                  ///< Generated text
    bool success = false;              ///< Whether generation was successful
    std::string errorMessage;          ///< Error message if generation failed
    translation::InferenceStats stats; ///< Generation statistics
};

/**
 * @class LLMModel
 * @brief Implementation of an LLM model using GGML
 *
 * This class extends the GGMLModel to provide specific LLM functionality
 * including text completion, chat, and other LLM-specific operations.
 */
class LLMModel : public models::GGMLModel {
public:
    friend class koebridge::llm::testing::LLMModelTest;
    friend class koebridge::llm::testing::NLLBModelTest;

    /**
     * @brief Constructor for LLMModel
     * @param modelInfo Information about the model to load
     * @param config Configuration options for the LLM
     */
    LLMModel(const translation::ModelInfo& modelInfo, const LLMConfig& config = LLMConfig());

    /**
     * @brief Destructor for LLMModel
     */
    ~LLMModel();

    /**
     * @brief Initialize the LLM model
     * @return bool True if initialization was successful
     */
    bool initialize() override;

    /**
     * @brief Generate text completion from a prompt
     * @param prompt The input prompt to complete
     * @return LLMOutput The generated completion and metadata
     */
    LLMOutput complete(const std::string& prompt);

    /**
     * @brief Generate text completion asynchronously
     * @param prompt The input prompt to complete
     * @return std::future<LLMOutput> Future containing the generated completion
     */
    std::future<LLMOutput> completeAsync(const std::string& prompt);

    /**
     * @brief Set configuration options for the LLM
     * @param config New configuration options
     */
    void setConfig(const LLMConfig& config);

    /**
     * @brief Get the current configuration
     * @return LLMConfig Current configuration options
     */
    LLMConfig getConfig() const;

    /**
     * @brief Format the input prompt according to the model's requirements
     * @param prompt The input prompt to format
     * @return std::string The formatted prompt
     */
    virtual std::string formatPrompt(const std::string& prompt);

    /**
     * @brief Convert text to token IDs using the local tokenizer
     * @param text Input text to tokenize
     * @return std::vector<int> Vector of token IDs
     */
    virtual std::vector<int> localTokenize(const std::string& text);

    /**
     * @brief Convert token IDs back to text using the local tokenizer
     * @param tokens Vector of token IDs to detokenize
     * @return std::string The detokenized text
     */
    virtual std::string localDetokenize(const std::vector<int>& tokens);

protected:
    /**
     * @brief Apply the given prompt to the model
     * @param prompt The input prompt
     * @param output Vector to store the generated tokens
     * @param stats Statistics about the generation
     * @return bool True if generation was successful
     */
    bool runGeneration(const std::string& prompt, std::vector<int>& output, translation::InferenceStats& stats);

private:
    LLMConfig config_;                  ///< Configuration options
};

} // namespace llm
} // namespace koebridge