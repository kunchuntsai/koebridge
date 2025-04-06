# KoeBridge (声ブリッジ)

![Version](https://img.shields.io/badge/version-0.1.0-blue.svg)
![License](https://img.shields.io/badge/license-MIT-green.svg)
![macOS](https://img.shields.io/badge/verified%20on-macOS-brightgreen.svg)

Real-time Japanese to English meeting translator using local LLMs. "Koe" means "voice" in Japanese, and "Bridge" represents the connection between languages. It's a fitting name for a project that bridges Japanese and English in real-time conversations.

## Features

- Real-time Japanese speech recognition
- Local translation using optimized LLMs
- Low-latency operation for natural conversation flow
- Optimized for Apple Silicon (M4)

## Prerequisites

- macOS 10.15 or later
- CMake 3.15 or later
- C++17 compatible compiler
- PortAudio 19.7.0+ (for audio capture)
- SentencePiece
- Qt 6 with the following components:
   - Core
   - Gui
   - Widgets
   - Concurrent
- Google Test 1.16.0+

## Installation

1. Clone the repository:
```bash
git clone https://github.com/yourusername/koebridge.git
cd koebridge
```

2. Set up the development environment:
```bash
./scripts/local_setup.sh
```

This will:
- Install required dependencies using Homebrew
- Set up the build environment
- Create necessary configuration files
- Initialize submodules

## Building

### Basic Build
```bash
./scripts/build.sh
```

### Build Options
- `--debug`: Build with debug symbols
- `--release`: Build in release mode (default)
- `--tests`: Build with tests enabled
- `--clean`: Clean build directory before building

Example:
```bash
# Clean build with debug symbols
./scripts/build.sh --clean --debug

# Build with tests enabled
./scripts/build.sh --tests
```

## Testing

KoeBridge includes a comprehensive test suite to ensure code quality and functionality.

### Running Tests

```bash
# Run all tests
./scripts/run.sh --tests

# Run specific test modules
./scripts/run.sh --tests --audio     # Audio tests only
./scripts/run.sh --tests --translation  # Translation tests only

# Clean build and run tests
./scripts/run.sh --tests --clean
```

For detailed information about the test suite, writing tests, and test structure, see the [Test Documentation](tests/unit/TEST_README.md).

## Running the Application

```bash
# Run the application (builds if needed)
./scripts/run.sh

# Run in debug mode
./scripts/run.sh --debug

# Force rebuild before running
./scripts/run.sh --build

# Clean build and run
./scripts/run.sh --clean
```

## Project Structure

```
koebridge/
├── include/             # Public header files
├── src/                 # Source files
│   ├── audio/           # Audio capture and processing
│   ├── translation/     # Translation service
│   └── utils/           # Utility functions
├── tests/               # Test files
│   └── unit/            # Unit tests
│       ├── audio/       # Audio-related tests
│       └── translation/ # Translation-related tests
├── third_party/         # Third-party dependencies
└── scripts/             # Build and utility scripts
```

## License

This project is licensed under the MIT License - see the LICENSE file for details.