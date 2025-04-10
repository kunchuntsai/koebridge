# Unit Tests

This directory contains unit tests for the KoeBridge project. The tests are organized by module and use the Google Test framework.

## Test Structure

```
tests/unit/
├── audio/              # Audio capture and processing tests
├── translation/        # Translation service tests
└── models/            # Model implementation tests
```

## Running Tests

### Run All Tests
```bash
./scripts/run.sh --tests
```

### Run Specific Test Modules
```bash
# Run audio tests only
./scripts/run.sh --tests --audio

# Run translation tests only
./scripts/run.sh --tests --translation
```

### Test Options

- `--debug`: Run tests in debug mode with debug symbols
- `--clean`: Clean build directory before building and running tests
- Combined example:
  ```bash
  # Clean build, run tests with debug symbols
  ./scripts/run.sh --tests --debug --clean
  ```

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

## Debugging Tests

1. Run tests with debug symbols:
```bash
./scripts/run.sh --tests --debug
```

2. Use GDB for debugging:
```bash
gdb ./build/tests/unit/audio/audio_capture_test
```

3. Set breakpoints and run:
```bash
(gdb) break ModuleTest::TestName
(gdb) run
```

## Continuous Integration

Tests are automatically run in the CI pipeline. See `.github/workflows/ci.yml` for details.