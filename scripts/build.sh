#!/bin/bash

# Function to display usage information
show_usage() {
    echo "Usage: $0 [OPTIONS]"
    echo "Build the koebridge project"
    echo ""
    echo "Options:"
    echo "  -c, --clean    Clean build (remove and recreate build directory)"
    echo "  -h, --help     Display this help message"
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
check_command cmake
check_command make
check_command pkg-config

# Check for required packages
check_brew_package qt@6
check_brew_package portaudio
check_brew_package googletest

# Check for required libraries
check_library portaudio-2.0

echo "All dependencies found."

# Change to project root directory
cd "$PROJECT_ROOT"

# Clean build directory if requested
if [ "$CLEAN" = true ]; then
    echo "Performing clean build..."
    rm -rf build
fi

# Create build directory if it doesn't exist
if [ ! -d "build" ]; then
    mkdir build
fi

# Navigate to build directory
cd build

# Run cmake and make
cmake ..
make

# Check build status
if [ $? -eq 0 ]; then
    echo "Build completed successfully!"
else
    echo "Build failed. See error messages above."
    exit 1
fi