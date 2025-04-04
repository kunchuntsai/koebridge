#!/bin/bash

# Function to display usage information
show_usage() {
    echo "Usage: $0 [OPTIONS]"
    echo "Run the koebridge application"
    echo ""
    echo "Options:"
    echo "  -d, --debug      Run in debug mode with additional logging"
    echo "  -h, --help       Display this help message"
}

# Get the directory where the script is located
SCRIPT_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
# Get the project root directory (parent of scripts directory)
PROJECT_ROOT="$( cd "$SCRIPT_DIR/.." && pwd )"

# Default values
DEBUG_MODE=false

# Parse command line arguments
while [[ $# -gt 0 ]]; do
    case "$1" in
        -d|--debug)
            DEBUG_MODE=true
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

# Change to project root directory
cd "$PROJECT_ROOT"

# Check if build directory exists
if [ ! -d "build" ]; then
    echo "Error: Build directory not found. Please build the project first."
    echo "Run './scripts/build.sh' to build the project."
    exit 1
fi

# Check if executable exists
if [ ! -f "build/koebridge" ]; then
    echo "Error: Executable not found. Please build the project first."
    echo "Run './scripts/build.sh' to build the project."
    exit 1
fi

# Run the application
echo "Starting KoeBridge..."
if [ "$DEBUG_MODE" = true ]; then
    echo "Debug mode enabled"
    KOEBRIDGE_DEBUG=1 ./build/koebridge
else
    ./build/koebridge
fi