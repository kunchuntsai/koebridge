# Unit Tests

This directory contains unit tests for the KoeBridge project. The tests are organized by module and use the Google Test framework.

## Directory Structure

```
tests/unit/
├── audio/             # Audio capture and processing tests
├── translation/       # Translation service tests
├── models/            # Model implementation tests
└── README.md          # This file
```

## Test Dependencies

- Google Test Framework (1.16.0+)
- Qt 6 (for GUI and async functionality)
- PortAudio (for audio capture tests)

## Building Tests

```bash
./scripts/test.sh --clean        # Build & run all tests (default)
./scripts/test.sh --all          # Build & run all tests (default)

./scripts/test.sh --audio        # Build & run only audio tests
./scripts/test.sh --translation  # Build & run only translation tests
```

## Running Tests

You can run the tests using the provided test script:

```bash
./unit_tests                     # Run all tests (default)
```

Additional options:
- `-c, --clean`: Clean build (remove and recreate build directory)
- `-h, --help`: Display help message

## Test Organization

### Audio Tests
- `audio_capture_test.cc`: Tests for audio input capture functionality
- `audio_processor_test.cc`: Tests for audio signal processing

### Translation Tests
- `translation_service_test.cc`: Tests for translation service functionality
- `model_manager_test.cc`: Tests for model management

## Writing Tests

When adding new tests:
1. Create a new test file in the appropriate module directory
2. Include the necessary test headers
3. Define test fixtures and test cases
4. Add the test file to CMakeLists.txt
5. Update the test script if adding a new module

## Test Guidelines

1. Each test should be self-contained and independent
2. Use test fixtures for common setup/teardown
3. Test both success and failure cases
4. Include meaningful test names and descriptions
5. Follow the existing test patterns and style

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