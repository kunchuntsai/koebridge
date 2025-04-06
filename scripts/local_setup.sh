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
if [ ! -d "third_party/ggml" ]; then
    echo "Adding GGML submodule..."
    git submodule add https://github.com/ggerganov/ggml.git third_party/ggml
fi

echo "Updating submodules..."
git submodule update --init --recursive

# Create config directory if it doesn't exist
echo "Setting up configuration..."
mkdir -p config

# Create default config.ini if it doesn't exist
if [ ! -f "config/config.ini" ]; then
    echo "Creating default config.ini..."
    cat > config/config.ini << EOL
[audio]
sample_rate = 16000
channels = 1
frames_per_buffer = 1024

[translation]
model_path = ./_dataset/models
default_model = nllb-ja-en

[ui]
window_width = 800
window_height = 600
theme = light
EOL
fi

# Create models directory
echo "Setting up models directory..."
mkdir -p _dataset/models

# Set up permissions
echo "Setting up permissions..."
chmod +x scripts/*.sh

echo "Local setup completed successfully!"
echo ""
echo "Next steps:"
echo "1. Run ./scripts/build.sh to build the project"
echo "2. Run ./scripts/run.sh to start the application"
echo ""
echo "Note: You may need to download translation models separately."
echo "See the README for instructions on model downloads." 