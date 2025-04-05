#pragma once

#include <ggml.h>
#include <vector>
#include <memory>
#include <string>

/**
 * @class TransformerLayer
 * @brief Represents a single transformer layer
 */
class TransformerLayer {
public:
    TransformerLayer(ggml_context* ctx, int hiddenSize, int numHeads);
    ~TransformerLayer();
    
    ggml_tensor* forward(ggml_tensor* input, ggml_tensor* attentionMask);
    
private:
    // Attention weights
    ggml_tensor* qkvWeight;
    ggml_tensor* qkvBias;
    ggml_tensor* outputWeight;
    ggml_tensor* outputBias;
    
    // FFN weights
    ggml_tensor* ffnWeight1;
    ggml_tensor* ffnBias1;
    ggml_tensor* ffnWeight2;
    ggml_tensor* ffnBias2;
    
    // Layer norm weights
    ggml_tensor* norm1Weight;
    ggml_tensor* norm1Bias;
    ggml_tensor* norm2Weight;
    ggml_tensor* norm2Bias;
    
    int hiddenSize;
    int numHeads;
};

/**
 * @class Transformer
 * @brief Complete transformer model implementation
 */
class Transformer {
public:
    Transformer(ggml_context* ctx, const std::string& modelPath);
    ~Transformer();
    
    bool loadModel(const std::string& modelPath);
    ggml_tensor* forward(ggml_tensor* input, ggml_tensor* attentionMask);
    
private:
    std::vector<std::unique_ptr<TransformerLayer>> layers;
    ggml_tensor* embeddingWeight;
    ggml_tensor* embeddingBias;
    ggml_tensor* normWeight;
    ggml_tensor* normBias;
    
    int numLayers;
    int hiddenSize;
    int numHeads;
    int vocabSize;
}; 