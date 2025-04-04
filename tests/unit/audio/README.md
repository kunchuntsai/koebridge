# Audio Tests

This directory contains tests for the audio capture and processing components of KoeBridge.

## Test Coverage

### AudioCapture Tests (`audio_capture_test.cc`)

The tests cover the following aspects of the AudioCapture class:

1. **Device Management**
   - Device enumeration
   - Device selection
   - Device information retrieval
   - Invalid device handling

2. **Audio Capture**
   - Real-time audio capture from microphone
   - Audio frame counting
   - Callback mechanism
   - Capture start/stop functionality

3. **Error Handling**
   - Invalid device selection
   - Capture without device selection
   - Error message reporting
   - Resource cleanup

4. **Resource Management**
   - Proper initialization
   - Clean shutdown
   - PortAudio resource cleanup
   - Memory management

## Testing Instructions

### Prerequisites

1. Install required dependencies:
   ```bash
   # On macOS
   brew install portaudio
   brew install googletest  # if not already installed
   ```

2. Ensure you have a working microphone connected to your system
3. Grant microphone access permissions to your terminal/IDE

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

2. Run only audio capture tests:
   ```bash
   ./unit_tests --gtest_filter=AudioCaptureTest.*
   ```

3. Run a specific test:
   ```bash
   ./unit_tests --gtest_filter=AudioCaptureTest.AudioCapture
   ```

### Test Output

The tests will output:
- List of available input devices
- Number of audio frames captured
- Any error messages or warnings

### Expected Results

1. **Device Enumeration Test**
   - Should list at least one input device
   - Should show device details (name, channels, sample rate)

2. **Device Selection Test**
   - Should successfully select first available device
   - Should reject invalid device indices

3. **Audio Capture Test**
   - Should capture audio for 2 seconds
   - Should receive more than 0 audio frames
   - Frame count should be approximately: `sample_rate * duration`

4. **Error Handling Test**
   - Should fail to start without device selection
   - Should provide meaningful error messages

### Troubleshooting

If tests fail:

1. **No Input Devices Found**
   - Check if microphone is connected
   - Verify system permissions
   - Check system audio settings

2. **Capture Fails**
   - Verify microphone is selected as input device
   - Check PortAudio installation
   - Ensure no other application is using the microphone

3. **Build Errors**
   - Verify PortAudio is installed
   - Check CMake configuration
   - Ensure all dependencies are available

## Adding New Tests

To add new tests:

1. Add test cases to `audio_capture_test.cc`
2. Follow the existing test structure
3. Include appropriate assertions
4. Document new test cases here

## Test Dependencies

- Google Test Framework
- PortAudio
- C++17 or later
- CMake 3.16 or later 