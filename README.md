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

Before building the project, ensure you have the following dependencies installed:

### Required Dependencies

1. CMake (3.16 or higher)
2. C++17 compatible compiler
3. Qt 6 with the following components:
   - Core
   - Gui
   - Widgets
   - Concurrent
4. PortAudio 19.7.0+ (for audio capture)
5. Google Test 1.16.0+ (for running tests)

### Installation on macOS

Using Homebrew:
```bash
# Install build tools
brew install cmake

# Install Qt 6
brew install qt@6

# Install PortAudio
brew install portaudio

# Install Google Test
brew install googletest
```

## Building

### Build Steps

You can use the included build script:

```bash
# Regular build
./scripts/build.sh

# Clean build (removes previous build artifacts)
./scripts/build.sh --clean

# Show build options
./scripts/build.sh --help
```

### Running the Application

After building, you can run KoeBridge using the provided run script:

```bash
# Regular execution
./scripts/run.sh

# Run in debug mode with additional logging
./scripts/run.sh --debug

# Show run options
./scripts/run.sh --help
```

### Generating Documentation

The project uses Doxygen for code documentation. You can generate the documentation using:

```bash
./scripts/doc-gen.sh
```

This will create HTML documentation in the `docs` directory. The documentation includes:
- Detailed API documentation for all classes and interfaces
- Class diagrams and call graphs
- Project structure and architecture overview
- Usage examples and implementation details

## Project Structure

The project is organized into several modules:

- `audio`: Audio capture and processing components
- `stt`: Speech-to-text conversion using Whisper
- `inference`: Core inference engine for model execution
- `models`: Implementation of supported model types (GGML, etc.)
- `translation`: Translation pipeline, model management and worker threads
- `ui`: Qt-based user interface components
- `utils`: Configuration, logging, and utility functions
- `scripts`: Build and run helper scripts

## Architecture

KoeBridge uses a modular architecture with clear separation of concerns:

- Audio capture → Speech recognition → Translation → UI display

The translation component uses a local LLM optimized for Japanese-English translation, with multiple model options available based on quality/speed requirements.

See the [design document](docs/design-doc.md) for detailed architecture information.

## Interfaces

Core interfaces are defined in the `include/interfaces` directory:
- `i_model_manager.h`: Manages model discovery, loading and lifecycle
- `i_translation_model.h`: Interface for different model implementations
- `i_translation_service.h`: High-level translation service API

## Troubleshooting

If you encounter build errors:

1. Make sure all dependencies are properly installed:
   ```bash
   # Check installed versions
   brew list --versions cmake
   brew list --versions qt@6
   brew list --versions portaudio
   brew list --versions googletest
   ```

2. Try running a clean build with `./scripts/build.sh --clean`
3. Check that your development environment supports C++17
4. Ensure all dependencies are correctly installed

## Contributing

Contributions are welcome! Please see our contributing guidelines for details.

## License

[MIT License](LICENSE)