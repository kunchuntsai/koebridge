# Unit Tests

This directory contains unit tests for the KoeBridge project. The tests are organized by module and use the Google Test framework.

## Test Structure

```
tests/unit/
├── audio/              # Audio capture and processing tests
├── translation/        # Translation service tests
├── models/            # Model implementation tests
├── llm/              # Language model tests
└── stt/              # Speech-to-Text tests
```

## Running Tests

There are multiple ways to run the tests:

### 1. Using the Run Script (Recommended)
```bash
# Run all tests
./scripts/run.sh --tests

# Run specific test modules
./scripts/run.sh --tests --audio       # Run audio tests only
./scripts/run.sh --tests --translation # Run translation tests only
./scripts/run.sh --tests --models      # Run model tests only
./scripts/run.sh --tests --stt         # Run STT tests only

# Run with options
./scripts/run.sh --tests --debug --clean  # Clean build and run with debug symbols
```

### 2. Using CTest (Alternative Method)
```bash
# Run all tests
ctest --output-on-failure

# Run only STT tests using CTest
# Method 1: Using test name pattern
ctest -R "stt" --output-on-failure

# Method 2: Using test labels
ctest -L "stt" --output-on-failure

# Method 3: Using specific test names
ctest -R "whisper_wrapper_test|realtime_transcriber_test" --output-on-failure

# Method 4: With verbose output
ctest -R "stt" -V --output-on-failure

# Method 5: Run tests in parallel
ctest -R "stt" -j4 --output-on-failure
```

### Test Options

#### Script Options
- `--debug`: Run tests in debug mode with debug symbols
- `--clean`: Clean build directory before building and running tests
- `--audio`: Run only audio-related tests
- `--translation`: Run only translation-related tests
- `--models`: Run only model-related tests
- `--stt`: Run only speech-to-text tests
- `--verbose`: Show detailed test output

#### CTest Options
- `--output-on-failure`: Show output for failed tests
- `-V`: Verbose output
- `-R pattern`: Run tests matching pattern (e.g., -R "stt" for STT tests)
- `-E pattern`: Exclude tests matching pattern
- `-j N`: Run N tests in parallel
- `-L label`: Run tests with specific label
- `--timeout N`: Set test timeout to N seconds
- `--stop-on-failure`: Stop on first test failure

### Troubleshooting Test Failures

1. Check test dependencies:
   - Google Test framework
   - PortAudio (for audio tests)
   - SentencePiece (for translation tests)
   - Qt6 (for UI components)
   - Whisper (for STT tests)

2. Common issues:
   - Missing libraries: Check CMake output for missing dependencies
   - Permission issues: Ensure model files are accessible
   - Resource conflicts: Close any applications using audio devices
   - Memory issues: Try running single tests in isolation
   - Missing Whisper model: Ensure the Whisper model file is available for STT tests

3. Debug failing tests:
   ```bash
   # Run specific test with GDB
   gdb ./build/tests/unit/stt/whisper_wrapper_test

   # In GDB
   (gdb) break TestClassName::TestName
   (gdb) run
   ```

4. View test logs:
   - Check `build/Testing/Temporary/LastTest.log`
   - Use `--verbose` option with run.sh
   - Run tests with `-V` option in CTest

## Writing Tests

### Test File Structure

```cpp
#include <gtest/gtest.h>
#include "module/header.h"

class ModuleTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Test setup code
    }

    void TearDown() override {
        // Test cleanup code
    }
};

TEST_F(ModuleTest, TestName) {
    // Test implementation
    EXPECT_TRUE(condition);
}
```

### Best Practices

1. Each test file should focus on a specific module or component
2. Use descriptive test names that explain the test's purpose
3. Group related tests using test fixtures
4. Clean up resources in TearDown()
5. Use appropriate test assertions (EXPECT_*, ASSERT_*)
6. Keep tests independent and isolated
7. Mock external dependencies when necessary

## Test Dependencies

- Google Test framework
- PortAudio (for audio tests)
- SentencePiece (for translation tests)
- Qt6 (for UI and async functionality)
- Whisper (for STT functionality)

## Debugging Tests

1. Run tests with debug symbols:
```bash
./scripts/run.sh --tests --debug
```

2. Use GDB for debugging:
```bash
gdb ./build/tests/unit/stt/whisper_wrapper_test
```

3. Set breakpoints and run:
```bash
(gdb) break TestClassName::TestName
(gdb) run
```

## Continuous Integration

Tests are automatically run in the CI pipeline. See `.github/workflows/ci.yml` for details.