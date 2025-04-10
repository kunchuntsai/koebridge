#!/bin/bash

# Exit on error
set -e

# Function to display usage information
show_usage() {
    echo "Usage: $0 [OPTIONS]"
    echo "Set up the local development environment for koebridge"
    echo ""
    echo "Options:"
    echo "  -c, --clean    Clean setup (remove and recreate directories)"
    echo "  -h, --help     Display this help message"
}

# Function to check if a command exists
command_exists() {
    command -v "$1" >/dev/null 2>&1
}

# Function to install a package using Homebrew
install_package() {
    local package=$1
    if ! brew list "$package" &>/dev/null; then
        echo "Installing $package..."
        brew install "$package"
    else
        echo "$package is already installed"
    fi
}

# Get the directory where the script is located
SCRIPT_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
# Get the project root directory (parent of scripts directory)
PROJECT_ROOT="$( cd "$SCRIPT_DIR/.." && pwd )"

# Parse command line arguments
CLEAN=false

while [[ $# -gt 0 ]]; do
    case "$1" in
        -c|--clean)
            CLEAN=true
            shift
            ;;
        -h|--help)
            show_usage
            exit 0
            ;;
        *)
            echo "Unknown option: $1"
            show_usage
            exit 1
            ;;
    esac
done

# Check for required commands
echo "Checking dependencies..."
command_exists brew
command_exists git
command_exists cmake
command_exists make
command_exists pkg-config

# Install required packages
echo "Installing required packages..."
install_package "cmake"
install_package "portaudio"
install_package "sentencepiece"
install_package "googletest"
install_package "qt@6"

# Check for required libraries
echo "Checking required libraries..."
if ! pkg-config --exists portaudio-2.0; then
    echo "Warning: portaudio-2.0 library not found via pkg-config"
    echo "This may cause issues with audio capture"
else
    echo "portaudio-2.0 found"
fi

# Change to project root directory
cd "$PROJECT_ROOT"

# Clean setup if requested
if [ "$CLEAN" = true ]; then
    echo "Performing clean setup..."
    rm -rf third_party
    rm -rf build
fi

# Create necessary directories
echo "Creating necessary directories..."
mkdir -p third_party
mkdir -p build

# Initialize and update submodules
echo "Initializing submodules..."
git submodule init
git submodule update --init --recursive

# Configure GGML and Whisper
echo "Configuring GGML and Whisper..."
cd third_party/ggml
git checkout master
mkdir -p build
cd build
cmake .. \
    -DGGML_METAL=OFF \
    -DGGML_USE_METAL=OFF \
    -DGGML_CPU=ON \
    -DGGML_BLAS=ON \
    -DGGML_OPENMP=ON \
    -DGGML_NATIVE=ON
make -j$(sysctl -n hw.ncpu)
cd ../..

cd whisper.cpp
git checkout master
mkdir -p build
cd build
cmake .. \
    -DWHISPER_METAL=OFF \
    -DWHISPER_USE_METAL=OFF \
    -DWHISPER_CPU=ON \
    -DWHISPER_BLAS=ON \
    -DWHISPER_OPENMP=ON \
    -DWHISPER_NATIVE=ON
make -j$(sysctl -n hw.ncpu)
cd ../..

# Return to project root
cd "$PROJECT_ROOT"

# Build the main project
echo "Building main project..."
cd build
cmake ..
make -j$(sysctl -n hw.ncpu)

echo "Setup completed successfully!"
echo ""
echo "Next steps:"
echo "1. Run ./scripts/build.sh to build the project"
echo "2. Run ./scripts/run.sh to start the application"
echo ""
echo "Note: You may need to download translation models separately."
echo "See the README for instructions on model downloads." 