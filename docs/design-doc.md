# KoeBridge Local LLM Design Document

## Table of Contents
- [1. User Journey](#1-user-journey)
  - [1.1 Initial Setup](#11-initial-setup)
  - [1.2 Regular Usage](#12-regular-usage)
  - [1.3 Advanced Usage](#13-advanced-usage)
- [2. Architecture and Building Blocks](#2-architecture-and-building-blocks)
  - [2.1 High-Level Architecture](#21-high-level-architecture)
  - [2.2 LLM Component Building Blocks](#22-llm-component-building-blocks)
- [3. Pipeline and Workflows](#3-pipeline-and-workflows)
  - [3.1 Translation Pipeline](#31-translation-pipeline)
  - [3.2 Model Loading Workflow](#32-model-loading-workflow)
  - [3.3 Translation Request Workflow](#33-translation-request-workflow)
- [4. Interfaces and Data Structures](#4-interfaces-and-data-structures)
  - [4.1 Core Interfaces](#41-core-interfaces)
  - [4.2 Key Data Structures](#42-key-data-structures)
- [5. Procedures](#5-procedures)
  - [5.1 Model Loading Procedure](#51-model-loading-procedure)
  - [5.2 Translation Procedure](#52-translation-procedure)
  - [5.3 Error Handling Procedure](#53-error-handling-procedure)
- [6. Implementation - Code Design](#6-implementation---code-design)
  - [6.1 Core Implementation Classes](#61-core-implementation-classes)
  - [6.2 Implementation Considerations](#62-implementation-considerations)
- [7. Verification - Test Cases](#7-verification---test-cases)
  - [7.1 Unit Tests](#71-unit-tests)
  - [7.2 Integration Tests](#72-integration-tests)
  - [7.3 Test Data Sets](#73-test-data-sets)
- [8. Verification - Test Reports and Known Issues](#8-verification---test-reports-and-known-issues)
  - [8.1 Performance Benchmarks](#81-performance-benchmarks)
  - [8.2 Translation Quality Metrics](#82-translation-quality-metrics)
  - [8.3 Known Issues](#83-known-issues)
  - [8.4 Future Improvements](#84-future-improvements)

## 1. User Journey

### 1.1 Initial Setup

1. **Application Installation**
   - User downloads and installs KoeBridge
   - First launch triggers initial setup wizard

2. **LLM Configuration**
   - User is presented with model options:
     - Fast (smaller model, lower quality)
     - Balanced (medium model, good quality/speed tradeoff)
     - High Quality (larger model, slower processing)
   - System recommends default based on device capabilities
   - User selects preferred model for download

3. **Model Download**
   - Application downloads selected model(s)
   - Progress bar indicates download and initialization status
   - Models are stored locally in user data directory

### 1.2 Regular Usage

1. **Application Launch**
   - User starts KoeBridge via `./scripts/run.sh`
   - Application loads configured LLM model in background
   - Status indicator shows when model is ready

2. **Translation Session**
   - User speaks or plays Japanese audio content
   - Whisper STT converts speech to Japanese text
   - Local LLM translates Japanese text to English
   - Both original text and translation are displayed in UI

3. **Model Switching**
   - User can change models via Settings dialog
   - Application handles unloading current model and loading new one
   - Translation quality/speed changes according to selected model

### 1.3 Advanced Usage

1. **Model Management**
   - Add, remove, or update translation models
   - View model statistics (size, performance, quality metrics)
   - Export/import custom models

2. **Translation Customization**
   - Adjust translation parameters (formality, verbosity)
   - Fine-tune model behavior for specific domains (general, technical, casual)
   - Save presets for different use cases

## 2. Architecture and Building Blocks

### 2.1 High-Level Architecture

```
┌───────────────────────────────────────────────────────┐
│                  KoeBridge Application                 │
├───────────┬───────────────┬───────────────────────────┤
│           │               │                           │
│  Audio    │    Speech     │      Translation          │
│  Capture  │    to Text    │      Engine               │
│  Module   │    Module     │                           │
│           │    (Whisper)  │                           │
│           │               │  ┌─────────────────────┐  │
├───────────┴───────────────┤  │                     │  │
│                           │  │   Model Manager     │  │
│                           │  │                     │  │
│        UI Module          │  ├─────────────────────┤  │
│                           │  │                     │  │
│                           │  │   Local LLM         │  │
│                           │  │   Engine            │  │
└───────────────────────────┘  │                     │  │
                               └─────────────────────┘  │
                                                        │
└────────────────────────────────────────────────────────┘
```

### 2.2 LLM Component Building Blocks

#### 2.2.1 Model Manager

Responsible for model lifecycle management:
- Model discovery and enumeration
- Model loading and initialization
- Resource allocation and cleanup
- Model metadata maintenance

#### 2.2.2 Inference Engine

Core translation logic:
- Text tokenization
- Model inference execution
- Output detokenization
- Performance optimization

#### 2.2.3 Translation Service

High-level API for application:
- Pre/post-processing
- Translation configuration
- Error handling and recovery
- Asynchronous operation

#### 2.2.4 Model Storage

Handles model files:
- Local model repository
- Version management
- Integrity verification
- Storage optimization

## 3. Pipeline and Workflows

### 3.1 Translation Pipeline

```
┌────────────┐    ┌────────────┐    ┌────────────┐    ┌────────────┐
│            │    │            │    │            │    │            │
│  Japanese  │─┬─▶│ Tokenizer  │───▶│ Inference  │───▶│ Detokenizer│─┬─▶ English
│  Text      │ │  │            │    │ Engine     │    │            │ │   Text
│            │ │  │            │    │            │    │            │ │
└────────────┘ │  └────────────┘    └────────────┘    └────────────┘ │
               │                                                      │
               │  ┌────────────┐    ┌────────────┐    ┌────────────┐ │
               │  │            │    │            │    │            │ │
               └─▶│ Text       │───▶│ Context    │───▶│ Post       │─┘
                  │ Normalization   │ Management │    │ Processing │
                  │            │    │            │    │            │
                  └────────────┘    └────────────┘    └────────────┘
```

### 3.2 Model Loading Workflow

```
┌─────────────┐    ┌─────────────┐    ┌─────────────┐
│ Check       │    │ Load Model  │    │ Initialize  │
│ Model       │───▶│ into        │───▶│ Inference   │
│ Availability│    │ Memory      │    │ Context     │
└─────────────┘    └─────────────┘    └─────────────┘
       │                                     │
       │ If not found                        │
       ▼                                     ▼
┌─────────────┐                      ┌─────────────┐
│ Prompt      │                      │ Create      │
│ for Model   │                      │ Tokenizer   │
│ Download    │                      │ & Processor │
└─────────────┘                      └─────────────┘
```

### 3.3 Translation Request Workflow

```
┌─────────────┐    ┌─────────────┐    ┌─────────────┐
│ Receive     │    │ Queue       │    │ Process     │
│ Japanese    │───▶│ Translation │───▶│ In Worker   │
│ Text        │    │ Request     │    │ Thread      │
└─────────────┘    └─────────────┘    └─────────────┘
                                            │
                                            ▼
┌─────────────┐    ┌─────────────┐    ┌─────────────┐
│ Emit        │    │ Apply Post- │    │ Execute     │
│ Translation │◀───│ Processing  │◀───│ Model       │
│ Signal      │    │ Rules       │    │ Inference   │
└─────────────┘    └─────────────┘    └─────────────┘
```

## 4. Interfaces and Data Structures

### 4.1 Core Interfaces

#### 4.1.1 Model Manager Interface

```cpp
class IModelManager {
public:
    virtual ~IModelManager() = default;
    
    // Model discovery
    virtual std::vector<ModelInfo> getAvailableModels() const = 0;
    virtual ModelInfo getActiveModel() const = 0;
    
    // Model lifecycle
    virtual bool loadModel(const std::string& modelId) = 0;
    virtual void unloadCurrentModel() = 0;
    virtual bool isModelLoaded() const = 0;
    
    // Model operations
    virtual std::shared_ptr<ITranslationModel> getTranslationModel() = 0;
    virtual bool downloadModel(const std::string& modelId, ProgressCallback callback) = 0;
};
```

#### 4.1.2 Translation Model Interface

```cpp
class ITranslationModel {
public:
    virtual ~ITranslationModel() = default;
    
    // Initialization
    virtual bool initialize() = 0;
    virtual bool isInitialized() const = 0;
    
    // Core operations
    virtual TranslationResult translate(const std::string& text, const TranslationOptions& options) = 0;
    virtual std::future<TranslationResult> translateAsync(const std::string& text, const TranslationOptions& options) = 0;
    
    // Status and metadata
    virtual ModelInfo getModelInfo() const = 0;
    virtual InferenceStats getLastInferenceStats() const = 0;
};
```

#### 4.1.3 Translation Service Interface

```cpp
class ITranslationService {
public:
    virtual ~ITranslationService() = default;
    
    // Service lifecycle
    virtual bool initialize() = 0;
    virtual void shutdown() = 0;
    
    // Translation operations
    virtual std::string translateText(const std::string& japaneseText) = 0;
    virtual void translateTextAsync(const std::string& japaneseText, TranslationCallback callback) = 0;
    
    // Configuration
    virtual void setTranslationOptions(const TranslationOptions& options) = 0;
    virtual TranslationOptions getTranslationOptions() const = 0;
    
    // Signals (implemented via Qt signals/slots)
    // void translationComplete(const std::string& sourceText, const std::string& translatedText);
    // void translationError(const std::string& sourceText, const std::string& errorMessage);
};
```

### 4.2 Key Data Structures

#### 4.2.1 Model Information

```cpp
struct ModelInfo {
    std::string id;              // Unique identifier
    std::string name;            // Display name
    std::string version;         // Version string
    std::string filePath;        // Path to model file
    int64_t sizeBytes;           // Model size in bytes
    ModelType type;              // GGML, ONNX, etc.
    std::map<std::string, std::string> metadata; // Additional info
};

enum class ModelType {
    GGML,
    ONNX,
    TENSORRT,
    CUSTOM
};
```

#### 4.2.2 Translation Options

```cpp
struct TranslationOptions {
    float temperature = 0.7f;    // Sampling temperature
    int maxLength = 1024;        // Maximum output length
    int beamSize = 4;            // Beam search size
    bool streamOutput = false;   // Stream partial results
    
    enum class Style {           // Translation style
        LITERAL,
        NATURAL,
        FORMAL,
        CASUAL
    } style = Style::NATURAL;
    
    int timeoutMs = 30000;       // Translation timeout
};
```

#### 4.2.3 Translation Result

```cpp
struct TranslationResult {
    std::string sourceText;      // Original Japanese text
    std::string translatedText;  // Translated English text
    bool success = false;        // Success flag
    std::string errorMessage;    // Error message if any
    
    struct Metrics {
        double totalTimeMs;      // Total processing time
        double inferenceTimeMs;  // Model inference time
        double preprocessTimeMs; // Pre-processing time
        double postprocessTimeMs;// Post-processing time
        int inputTokenCount;     // Number of input tokens
        int outputTokenCount;    // Number of output tokens
    } metrics;
};
```

## 5. Procedures

### 5.1 Model Loading Procedure

1. **Check Model Availability**
   - Verify model file exists in expected location
   - Check model file integrity (checksum)
   - Verify compatibility with current runtime

2. **Memory Mapping**
   - Memory map model file for efficient loading
   - Allocate memory buffer for model weights
   - Handle memory allocation errors

3. **Model Initialization**
   - Initialize inference context
   - Load vocabulary and tokenizer
   - Set up thread pool for inference
   - Perform test inference to verify functionality

### 5.2 Translation Procedure

1. **Pre-processing**
   - Normalize input text (whitespace, punctuation)
   - Apply text segmentation for long inputs
   - Insert special tokens or prompts

2. **Inference Execution**
   - Tokenize input text
   - Execute forward pass through model
   - Handle attention caching for efficiency
   - Implement sampling strategy (temperature, top-k, top-p)

3. **Post-processing**
   - Detokenize output
   - Apply formatting rules
   - Filter out special tokens
   - Format final translation

### 5.3 Error Handling Procedure

1. **Input Validation**
   - Check for empty or invalid input
   - Verify input length is within model constraints
   - Handle non-Japanese input gracefully

2. **Runtime Error Recovery**
   - Detect model inference failures
   - Implement retry mechanism with backoff
   - Fall back to alternative models if available
   - Provide meaningful error messages to UI

3. **Resource Management**
   - Monitor memory usage during translation
   - Implement context recycling for efficiency
   - Release resources after translation completion

## 6. Implementation - Code Design

### 6.1 Core Implementation Classes

#### 6.1.1 Model Manager Implementation

```cpp
// model_manager.h
class ModelManager : public IModelManager {
public:
    ModelManager(const std::string& modelDir);
    ~ModelManager() override;
    
    std::vector<ModelInfo> getAvailableModels() const override;
    ModelInfo getActiveModel() const override;
    
    bool loadModel(const std::string& modelId) override;
    void unloadCurrentModel() override;
    bool isModelLoaded() const override;
    
    std::shared_ptr<ITranslationModel> getTranslationModel() override;
    bool downloadModel(const std::string& modelId, ProgressCallback callback) override;

private:
    std::string modelDir_;
    std::vector<ModelInfo> availableModels_;
    std::shared_ptr<ITranslationModel> currentModel_;
    ModelInfo activeModelInfo_;
    
    void scanAvailableModels();
    std::shared_ptr<ITranslationModel> createModelInstance(const ModelInfo& info);
};
```

#### 6.1.2 GGML Model Implementation

```cpp
// ggml_model.h
class GGMLModel : public ITranslationModel {
public:
    GGMLModel(const ModelInfo& modelInfo);
    ~GGMLModel() override;
    
    bool initialize() override;
    bool isInitialized() const override;
    
    TranslationResult translate(const std::string& text, const TranslationOptions& options) override;
    std::future<TranslationResult> translateAsync(const std::string& text, const TranslationOptions& options) override;
    
    ModelInfo getModelInfo() const override;
    InferenceStats getLastInferenceStats() const override;

private:
    ModelInfo modelInfo_;
    bool initialized_ = false;
    InferenceStats lastStats_;
    
    // GGML specific members
    void* model_ = nullptr;
    void* ctx_ = nullptr;
    
    // Thread pool for async operations
    std::shared_ptr<ThreadPool> threadPool_;
    
    // Internal methods
    void tokenize(const std::string& text, std::vector<int>& tokens);
    std::string detokenize(const std::vector<int>& tokens);
    void runInference(const std::vector<int>& inputTokens, std::vector<int>& outputTokens, const TranslationOptions& options);
};
```

#### 6.1.3 Translation Service Implementation

```cpp
// translation_service.h
class TranslationService : public QObject, public ITranslationService {
    Q_OBJECT
    
public:
    TranslationService(std::shared_ptr<IModelManager> modelManager);
    ~TranslationService() override;
    
    bool initialize() override;
    void shutdown() override;
    
    std::string translateText(const std::string& japaneseText) override;
    void translateTextAsync(const std::string& japaneseText, TranslationCallback callback) override;
    
    void setTranslationOptions(const TranslationOptions& options) override;
    TranslationOptions getTranslationOptions() const override;

signals:
    void translationComplete(const std::string& sourceText, const std::string& translatedText);
    void translationError(const std::string& sourceText, const std::string& errorMessage);
    void translationProgress(int percentComplete);

private:
    std::shared_ptr<IModelManager> modelManager_;
    TranslationOptions options_;
    std::atomic<bool> initialized_ = false;
    
    // Worker thread and queue
    std::unique_ptr<QThread> workerThread_;
    std::unique_ptr<TranslationWorker> worker_;
    
    // Helper methods
    std::string preprocess(const std::string& text);
    std::string postprocess(const std::string& text);
};
```

### 6.2 Implementation Considerations

#### 6.2.1 Memory Management

- **Memory Mapping**: Use memory mapping for large model files
- **Quantization**: Support INT8 and INT4 quantized models
- **Weight Sharing**: Implement weight sharing between model instances
- **Garbage Collection**: Implement proper cleanup of temporary resources

#### 6.2.2 Performance Optimization

- **Thread Pool**: Use thread pool for parallel inference
- **Batching**: Implement batched processing for multiple sentences
- **KV Cache**: Implement key-value cache for efficient sequential processing
- **SIMD Acceleration**: Utilize SIMD instructions (AVX2, NEON) for inference

#### 6.2.3 Apple Silicon Optimization

- **Metal Support**: Implement Metal acceleration for Apple Silicon
- **ANE Integration**: Explore Apple Neural Engine integration for supported models
- **Core ML**: Consider Core ML backend for optimized inference
- **Performance Profiles**: Create specific optimizations for M1/M2/M4 chips

## 7. Verification - Test Cases

### 7.1 Unit Tests

#### 7.1.1 Model Manager Tests

- **Test Model Discovery**: Verify correct enumeration of available models
- **Test Model Loading**: Verify successful loading of models
- **Test Model Switching**: Verify correct unloading/loading when switching
- **Test Error Handling**: Verify correct behavior with missing/corrupt models

#### 7.1.2 Translation Model Tests

- **Test Initialization**: Verify successful model initialization
- **Test Basic Translation**: Verify correct translation of simple sentences
- **Test Long Text**: Verify handling of texts longer than context window
- **Test Special Characters**: Verify handling of special characters and formatting
- **Test Error Cases**: Verify graceful handling of invalid inputs

#### 7.1.3 Translation Service Tests

- **Test End-to-End**: Verify complete translation pipeline
- **Test Async Operation**: Verify asynchronous translation requests
- **Test Cancellation**: Verify request cancellation functionality
- **Test Multiple Requests**: Verify handling of multiple concurrent requests

### 7.2 Integration Tests

- **STT Integration**: Verify seamless interaction with speech recognition
- **UI Integration**: Verify correct display of translations in UI
- **Performance Tests**: Verify translation speed meets requirements
- **Resource Usage**: Verify memory and CPU usage within acceptable limits

### 7.3 Test Data Sets

- **Standard Phrases**: Common Japanese phrases with known translations
- **Technical Content**: Domain-specific technical content
- **Conversational**: Natural dialogue samples
- **Edge Cases**: Long sentences, rare characters, unusual grammar
- **Real-world Recordings**: Actual meeting transcripts for end-to-end testing

## 8. Verification - Test Reports and Known Issues

### 8.1 Performance Benchmarks

| Model Size | Avg. Translation Time | Memory Usage | CPU Usage |
|------------|----------------------|--------------|-----------|
| Small      | 0.2-0.5s / sentence  | ~200MB       | 30-40%    |
| Medium     | 0.5-1.0s / sentence  | ~500MB       | 50-70%    |
| Large      | 1.0-2.0s / sentence  | ~1.2GB       | 70-90%    |

### 8.2 Translation Quality Metrics

| Model Size | BLEU Score | Human Evaluation | Adequacy (1-5) | Fluency (1-5) |
|------------|------------|------------------|----------------|---------------|
| Small      | 18-22      | Acceptable       | 3.2            | 3.4           |
| Medium     | 24-28      | Good             | 4.1            | 4.3           |
| Large      | 30-34      | Excellent        | 4.7            | 4.8           |

### 8.3 Known Issues

1. **Context Limitations**
   - **Issue**: Long conversations may lose context beyond model window size
   - **Severity**: Medium
   - **Workaround**: Implement sliding context window with overlap
   - **Fix Status**: Planned for version 0.2.0

2. **Specialized Terminology**
   - **Issue**: Technical or domain-specific terms may be mistranslated
   - **Severity**: Medium
   - **Workaround**: Add custom dictionary for domain-specific terms
   - **Fix Status**: In progress

3. **Memory Consumption**
   - **Issue**: High memory usage with large models on low-memory devices
   - **Severity**: High
   - **Workaround**: Automatically fall back to smaller models on low-memory devices
   - **Fix Status**: Implemented in latest version

4. **Initial Loading Time**
   - **Issue**: First-time model loading can take 5-10 seconds
   - **Severity**: Low
   - **Workaround**: Show loading progress and preload models at startup
   - **Fix Status**: Partially mitigated

5. **Rare Character Handling**
   - **Issue**: Some rare Japanese characters may cause translation errors
   - **Severity**: Low
   - **Workaround**: Pre-process text to replace rare characters
   - **Fix Status**: Under investigation

### 8.4 Future Improvements

1. **Model Compression**
   - Implement more aggressive quantization techniques
   - Explore pruning and distillation for smaller models

2. **Hardware Acceleration**
   - Add support for CUDA/ROCm for systems with GPUs
   - Optimize Metal performance for Apple Silicon

3. **Personalization**
   - Add user glossary for custom translations
   - Implement fine-tuning for specific domains

4. **Multilingual Support**
   - Extend beyond Japanese-English to support additional language pairs
   - Implement multilingual models for broader language support

5. **Streaming Translation**
   - Implement real-time streaming translation with partial results
   - Reduce end-to-end latency for better conversational flow