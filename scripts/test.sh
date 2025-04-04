#!/bin/bash

# Function to display usage information
show_usage() {
    echo "Usage: $0 [OPTIONS]"
    echo "Build and run tests for the koebridge project"
    echo ""
    echo "Options:"
    echo "  -c, --clean    Clean build (remove and recreate build directory)"
    echo "  -h, --help     Display this help message"
    echo "  --audio        Run only audio-related tests"
    echo "  --translation  Run only translation-related tests"
    echo "  --all          Run all tests (default)"
}

# Get the directory where the script is located
SCRIPT_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
# Get the project root directory (parent of scripts directory)
PROJECT_ROOT="$( cd "$SCRIPT_DIR/.." && pwd )"

# Parse command line arguments
CLEAN=false
TEST_MODULE="all"

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
        --audio)
            TEST_MODULE="audio"
            shift
            ;;
        --translation)
            TEST_MODULE="translation"
            shift
            ;;
        --all)
            TEST_MODULE="all"
            shift
            ;;
        *)
            echo "Unknown option: $1"
            show_usage
            exit 1
            ;;
    esac
done

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

# Configure CMake with tests enabled
echo "Configuring CMake with tests enabled..."
cmake -DBUILD_TESTS=ON ..

# Build tests
echo "Building tests..."
make

# Run tests based on module selection
echo "Running tests..."
case "$TEST_MODULE" in
    "audio")
        ./unit_tests --gtest_filter=AudioCaptureTest.*
        ;;
    "translation")
        ./unit_tests --gtest_filter=TranslationServiceTest.*
        ;;
    "all")
        ./unit_tests
        ;;
esac

# Check build status
if [ $? -eq 0 ]; then
    echo "Tests completed successfully!"
else
    echo "Tests failed. See error messages above."
    exit 1
fi 