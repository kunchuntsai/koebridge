# Translation Module Tests

This directory contains unit tests for the translation module components.

## Test Coverage

### ModelManager Tests

The `ModelManager` class tests cover the following aspects:

1. **Model Discovery**
   - Finding available models in the model directory
   - Handling valid and invalid model files
   - Filtering out empty or corrupted files
   - Model metadata validation

2. **Model Lifecycle**
   - Loading models
   - Unloading models
   - Model state management
   - Active model tracking

3. **Error Handling**
   - Invalid model files
   - Non-existent models
   - Empty model files
   - Directory access errors
   - Resource cleanup

4. **Model Download**
   - Download progress reporting
   - Existing model handling
   - Download cancellation
   - Error cases

5. **Translation Model Integration**
   - Model initialization
   - Translation model creation
   - Resource management
   - Null handling

## Running Tests

You can run the translation tests using:

```bash
./scripts/test.sh --translation
```

## Test Structure

Each test file follows the Google Test framework structure:

```cpp
class TranslationTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Test setup
    }
    
    void TearDown() override {
        // Test cleanup
    }
};
```

## Test Files

- `model_manager_test.cc`: Tests for model management functionality
- `translation_service_test.cc`: Tests for translation service
- `model_manager_test.cc`: Tests for model management

## Dependencies

- Google Test Framework
- C++17 Standard Library
- Filesystem library

## Adding New Tests

When adding new tests:
1. Create a new test file in this directory
2. Follow the existing test structure
3. Include comprehensive test cases
4. Add proper cleanup in TearDown
5. Document test coverage in this README