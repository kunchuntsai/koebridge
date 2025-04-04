# Unit Tests

This directory contains unit tests for the KoeBridge project. The tests are organized by module and use the Google Test framework.

## Directory Structure

```
tests/unit/
├── audio/              # Audio capture and processing tests
│   ├── audio_capture_test.cc
│   └── README.md
├── translation/        # Translation service tests
│   └── model_manager_test.cc
├── models/            # Model implementation tests
│   └── ggml_model_test.cc
└── README.md          # This file
```

## Building and Running Tests

### Using the Test Script

The project provides a convenient script for building and running tests:

```bash
# Build and run all tests
./scripts/test.sh

# Clean build and run all tests
./scripts/test.sh --clean

# Build tests without running them
./scripts/test.sh --build

# Run tests without rebuilding
./scripts/test.sh --run

# Run specific test suite
./scripts/test.sh --filter AudioCaptureTest.*
```

### Manual Build

If you prefer to build manually:

```bash
# Create and enter build directory
mkdir -p build && cd build

# Configure with tests enabled
cmake -DBUILD_TESTS=ON ..

# Build
make

# Run tests
./unit_tests
```

## Test Categories

### Audio Tests
- Device enumeration and selection
- Audio capture functionality
- Error handling
- Resource management

### Translation Tests
- Model manager functionality
- Service initialization
- Translation pipeline

### Model Tests
- GGML model implementation
- Model loading and inference
- Performance benchmarks

## Writing New Tests

1. Create a new test file in the appropriate module directory
2. Follow the existing test structure:
   ```cpp
   class YourTest : public ::testing::Test {
   protected:
       void SetUp() override {
           // Test setup
       }

       void TearDown() override {
           // Test cleanup
       }
   };

   TEST_F(YourTest, TestName) {
       // Test implementation
       EXPECT_TRUE(condition);
   }
   ```

3. Add the test file to CMakeLists.txt in the appropriate test target

## Test Guidelines

1. **Test Organization**
   - Group related tests in test fixtures
   - Use descriptive test names
   - Follow the AAA pattern (Arrange, Act, Assert)

2. **Test Coverage**
   - Test both success and failure cases
   - Include edge cases
   - Test resource cleanup

3. **Test Independence**
   - Each test should be independent
   - Clean up resources in TearDown
   - Don't rely on test execution order

4. **Performance**
   - Keep tests fast
   - Use appropriate timeouts
   - Mock external dependencies

## Common Issues

1. **Build Failures**
   - Check if BUILD_TESTS is enabled
   - Verify all dependencies are installed
   - Check CMake configuration

2. **Test Failures**
   - Check test output for details
   - Verify test environment
   - Check for resource leaks

3. **Missing Tests**
   - Ensure test file is added to CMakeLists.txt
   - Check test naming convention
   - Verify test registration

## Dependencies

- Google Test Framework (googletest)
  - Required for test framework
  - Install via: `brew install googletest`
  - Version: 1.16.0 or later

- PortAudio
  - Required for audio capture tests
  - Install via: `brew install portaudio`
  - Version: 19.7.0 or later

- CMake 3.16 or later
- C++17 or later
- Module-specific dependencies

## Contributing

When adding new tests:
1. Follow the existing test structure
2. Add appropriate documentation
3. Update this README if adding new test categories
4. Ensure tests pass on all supported platforms 