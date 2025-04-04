# Translation Tests

This directory contains tests for the translation components of KoeBridge, including the TranslationService and ModelManager classes.

## Test Coverage

### TranslationService Tests (`translation_service_test.cc`)

The tests cover the following aspects of the TranslationService class:

1. **Service Lifecycle**
   - Service initialization
   - Service shutdown
   - Resource cleanup
   - State management

2. **Translation Operations**
   - Synchronous translation
   - Asynchronous translation
   - Translation options management
   - Translation result handling

3. **Error Handling**
   - Uninitialized service usage
   - Invalid model handling
   - Error message reporting
   - Resource cleanup

4. **Options Management**
   - Setting translation options
   - Retrieving current options
   - Option validation
   - Default options

## Testing Instructions

### Prerequisites

1. Install required dependencies:
   ```bash
   # On macOS
   brew install qt@6
   brew install googletest  # if not already installed
   ```

2. Ensure you have the test models directory set up:
   ```bash
   mkdir -p test_models
   ```

### Building Tests

1. Configure CMake with tests enabled:
   ```bash
   mkdir build && cd build
   cmake -DBUILD_TESTS=ON ..
   ```

2. Build the tests:
   ```bash
   make
   ```

### Running Tests

1. Run all tests:
   ```bash
   ./unit_tests
   ```

2. Run only translation service tests:
   ```bash
   ./unit_tests --gtest_filter=TranslationServiceTest.*
   ```

3. Run a specific test:
   ```bash
   ./unit_tests --gtest_filter=TranslationServiceTest.TestSynchronousTranslation
   ```

### Test Output

The tests will output:
- Service initialization status
- Translation results
- Any error messages or warnings
- Async operation completion status

### Expected Results

1. **Initialization Test**
   - Should successfully initialize the service
   - Should set default translation options
   - Should load default model

2. **Translation Test**
   - Should handle Japanese input text
   - Should return non-empty translation
   - Should maintain source text in result

3. **Async Translation Test**
   - Should complete within timeout period
   - Should invoke callback with result
   - Should handle errors appropriately

4. **Options Test**
   - Should correctly set all options
   - Should maintain option values
   - Should apply options to translations