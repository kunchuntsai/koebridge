#!/bin/bash

# Exit on error
set -e

# Function to display usage information
show_usage() {
    echo "Usage: $0 [OPTIONS]"
    echo "Build the koebridge project"
    echo ""
    echo "Options:"
    echo "  -c, --clean    Clean build (remove build directory)"
    echo "  -h, --help     Display this help message"
    echo "  --all          Build both app and tests"
    echo "  --debug        Build in debug mode"
    echo "  --release      Build in release mode"
    echo "  --tests        Build tests"
    echo "  --tests-run    Build and run tests"
}

# Function to check if a command exists
check_command() {
    if ! command -v $1 &> /dev/null; then
        echo "Error: $1 is not installed"
        echo "Please install it using: brew install $1"
        exit 1
    fi
}

# Function to check if a package is installed via brew
check_brew_package() {
    if ! brew list $1 &> /dev/null; then
        echo "Error: $1 is not installed"
        echo "Please install it using: brew install $1"
        exit 1
    fi
}

# Function to check if a library exists
check_library() {
    if ! pkg-config --exists $1; then
        echo "Error: $1 library not found"
        echo "Please install it using: brew install $1"
        exit 1
    fi
}

# Function to check if a directory is writable
check_writable() {
    if [ ! -w "$1" ]; then
        echo "Error: Directory $1 is not writable"
        echo "Please ensure you have write permissions to this directory"
        exit 1
    fi
}

# Default values
BUILD_TYPE="Release"
BUILD_TESTS="OFF"
RUN_TESTS="OFF"
CLEAN_BUILD=false

# Parse arguments
while [[ $# -gt 0 ]]; do
    case $1 in
        --debug)
            BUILD_TYPE="Debug"
            shift
            ;;
        --release)
            BUILD_TYPE="Release"
            shift
            ;;
        --all)
            BUILD_TESTS="ON"
            shift
            ;;
        --tests)
            BUILD_TESTS="ON"
            shift
            ;;
        --tests-run)
            BUILD_TESTS="ON"
            RUN_TESTS="ON"
            shift
            ;;
        --clean)
            CLEAN_BUILD=true
            shift
            ;;
        -h|--help)
            show_usage
            exit 0
            ;;
        *)
            echo "Unknown option: $1"
            echo "Usage: $0 [--debug|--release] [--all|--tests|--tests-run] [--clean]"
            exit 1
            ;;
    esac
done

# Check for required commands
echo "Checking dependencies..."
check_command brew
check_command git
check_command cmake
check_command make
check_command pkg-config

# Check for required packages
echo "Checking required packages..."
check_brew_package qt@6
check_brew_package portaudio
check_brew_package googletest
check_brew_package sentencepiece

# Check for required libraries
echo "Checking required libraries..."
check_library portaudio-2.0

# Get the directory where the script is located
SCRIPT_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
PROJECT_ROOT="$(dirname "$SCRIPT_DIR")"

# Change to project root directory
cd "$PROJECT_ROOT"

# Check if we have write permissions
check_writable "$PROJECT_ROOT"

# Clean build directory if requested
if [ "$CLEAN_BUILD" = true ]; then
    echo "Cleaning build directory..."
    rm -rf build
fi

# Create build directory if it doesn't exist
mkdir -p build
cd build

# Configure CMake with GGML Metal backend disabled
echo "Configuring CMake..."
cmake .. \
    -DCMAKE_BUILD_TYPE="$BUILD_TYPE" \
    -DBUILD_TESTS="$BUILD_TESTS" \
    -DGGML_METAL=OFF \
    -DGGML_USE_METAL=OFF

# Build the project
echo "Building project..."
cmake --build . -- -j$(nproc 2>/dev/null || sysctl -n hw.ncpu 2>/dev/null || echo 2)

# Run tests if requested
if [ "$RUN_TESTS" = "ON" ]; then
    echo "Running tests..."
    ctest --output-on-failure
fi

echo "Build completed successfully!"
if [ "$RUN_TESTS" = "ON" ]; then
    echo "Tests completed successfully!"
fi