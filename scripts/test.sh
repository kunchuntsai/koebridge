#!/bin/bash

# Function to display usage information
show_usage() {
    echo "Usage: $0 [OPTIONS]"
    echo "Build and run koebridge tests"
    echo ""
    echo "Options:"
    echo "  -c, --clean    Clean build (remove and recreate build directory)"
    echo "  -b, --build    Only build tests without running them"
    echo "  -r, --run      Only run tests without rebuilding"
    echo "  -f, --filter   Filter tests to run (e.g., AudioCaptureTest.*)"
    echo "  -h, --help     Display this help message"
}

# Get the directory where the script is located
SCRIPT_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
# Get the project root directory (parent of scripts directory)
PROJECT_ROOT="$( cd "$SCRIPT_DIR/.." && pwd )"

# Parse command line arguments
CLEAN=false
BUILD_ONLY=false
RUN_ONLY=false
TEST_FILTER=""

while [[ $# -gt 0 ]]; do
    case "$1" in
        -c|--clean)
            CLEAN=true
            shift
            ;;
        -b|--build)
            BUILD_ONLY=true
            shift
            ;;
        -r|--run)
            RUN_ONLY=true
            shift
            ;;
        -f|--filter)
            TEST_FILTER="$2"
            shift 2
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

# Build tests if not only running
if [ "$RUN_ONLY" = false ]; then
    echo "Configuring CMake with tests enabled..."
    cmake -DBUILD_TESTS=ON ..
    
    echo "Building tests..."
    make
    
    # Check build status
    if [ $? -ne 0 ]; then
        echo "Build failed. See error messages above."
        exit 1
    fi
fi

# Run tests if not only building
if [ "$BUILD_ONLY" = false ]; then
    echo "Running tests..."
    if [ -n "$TEST_FILTER" ]; then
        ./unit_tests --gtest_filter="$TEST_FILTER"
    else
        ./unit_tests
    fi
    
    # Check test status
    if [ $? -ne 0 ]; then
        echo "Tests failed. See error messages above."
        exit 1
    fi
fi

echo "Test process completed successfully!" 