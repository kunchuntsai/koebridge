#!/bin/bash

# Function to display usage information
show_usage() {
    echo "Usage: $0 [OPTIONS]"
    echo "Run the koebridge application or tests"
    echo ""
    echo "Options:"
    echo "  -d, --debug      Run in debug mode with additional logging"
    echo "  -h, --help       Display this help message"
    echo "  --tests          Run the tests instead of the application"
    echo "  --audio          Run only audio tests (must be used with --tests)"
    echo "  --translation    Run only translation tests (must be used with --tests)"
    echo "  --stt            Run only speech-to-text tests (must be used with --tests)"
    echo "  --clean          Clean the build directory before building"
    echo "  --build          Force rebuild before running (implied with --clean)"
}

# Get the directory where the script is located
SCRIPT_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
# Get the project root directory (parent of scripts directory)
PROJECT_ROOT="$( cd "$SCRIPT_DIR/.." && pwd )"

# Default values
DEBUG_MODE=false
RUN_TESTS=false
CLEAN_BUILD=false
FORCE_BUILD=false
TEST_FILTER="*"

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
        --tests)
            RUN_TESTS=true
            shift
            ;;
        --audio)
            TEST_FILTER="audio*"
            shift
            ;;
        --translation)
            TEST_FILTER="translation*"
            shift
            ;;
        --stt)
            TEST_FILTER="stt*"
            shift
            ;;
        --clean)
            CLEAN_BUILD=true
            FORCE_BUILD=true
            shift
            ;;
        --build)
            FORCE_BUILD=true
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

# Determine build type based on debug mode
if [ "$DEBUG_MODE" = true ]; then
    BUILD_TYPE="Debug"
    BUILD_TYPE_LOWER="debug"
else
    BUILD_TYPE="Release"
    BUILD_TYPE_LOWER="release"
fi

# Build function to avoid code duplication
function build_project() {
    local build_tests=$1
    local build_clean=$CLEAN_BUILD

    echo "Building the project..."
    
    # Set build command
    local build_cmd="./scripts/build.sh --$BUILD_TYPE_LOWER"
    
    # Add tests flag if needed
    if [ "$build_tests" = true ]; then
        build_cmd="$build_cmd --tests"
    fi
    
    # Add clean flag if needed
    if [ "$build_clean" = true ]; then
        build_cmd="$build_cmd --clean"
    fi
    
    # Execute build command
    echo "Running: $build_cmd"
    eval $build_cmd
    
    # Check if build was successful
    if [ $? -ne 0 ]; then
        echo "Build failed."
        return 1
    fi
    
    return 0
}

# If we're running tests
if [ "$RUN_TESTS" = true ]; then
    echo "Preparing to run tests..."
    
    # Determine which test executables to check based on filter
    TEST_TO_CHECK=""
    if [ "$TEST_FILTER" = "audio*" ]; then
        TEST_TO_CHECK="audio_capture_test"
    elif [ "$TEST_FILTER" = "translation*" ]; then
        TEST_TO_CHECK="translation_service_test"
    elif [ "$TEST_FILTER" = "stt*" ]; then
        TEST_TO_CHECK="whisper_wrapper_test"
    else
        TEST_TO_CHECK="audio_capture_test"  # Default check for all tests
    fi
    
    # Check if specific test executable exists or force build is set
    if [ ! -d "build" ] || [ ! -f "build/tests/$TEST_TO_CHECK" ] || [ "$FORCE_BUILD" = true ]; then
        # Build with tests enabled
        build_project true
        if [ $? -ne 0 ]; then
            echo "Test build failed. Aborting test run."
            exit 1
        fi
    else
        echo "Required tests already built. Skipping build step."
    fi
    
    # Change to build directory
    cd build
    
    # Define test executables
    TEST_EXECUTABLES=(
        "tests/audio_capture_test"
        "tests/translation_service_test"
        "tests/model_manager_test"
        "tests/whisper_wrapper_test"
        "tests/realtime_transcriber_test"
    )
    
    # Run the tests directly instead of using ctest
    echo "Running tests..."
    
    # Initialize exit code
    exit_code=0
    
    # Filter tests if needed
    if [ "$TEST_FILTER" = "audio*" ]; then
        echo "Running audio tests..."
        if [ -f "tests/audio_capture_test" ]; then
            ./tests/audio_capture_test
        else
            echo "Audio test executable not found. It may not have been built."
        fi
    elif [ "$TEST_FILTER" = "translation*" ]; then
        echo "Running translation tests..."
        if [ -f "tests/translation_service_test" ]; then
            ./tests/translation_service_test
        fi
        if [ -f "tests/model_manager_test" ]; then
            ./tests/model_manager_test
        fi
    elif [ "$TEST_FILTER" = "stt*" ]; then
        echo "Running STT tests..."
        if [ -f "tests/whisper_wrapper_test" ]; then
            ./tests/whisper_wrapper_test
        fi
        if [ -f "tests/realtime_transcriber_test" ]; then
            ./tests/realtime_transcriber_test
        fi
    else
        # Run all tests
        echo "Running all tests..."
        for test in "${TEST_EXECUTABLES[@]}"; do
            if [ -f "$test" ]; then
                echo "Running $test..."
                ./$test
                if [ $? -ne 0 ]; then
                    echo "Test $test failed."
                    exit_code=1
                fi
            else
                echo "Test executable $test not found. It may not have been built."
            fi
        done
    fi
    
    exit $exit_code
else
    # Running the main application
    
    # Check if build directory exists or force build is set
    if [ ! -d "build" ] || [ ! -f "build/koebridge" ] || [ "$FORCE_BUILD" = true ]; then
        build_project false
        if [ $? -ne 0 ]; then
            echo "Application build failed. Cannot run the application."
            exit 1
        fi
    fi
    
    # Run the application
    echo "Starting KoeBridge..."
    if [ "$DEBUG_MODE" = true ]; then
        echo "Debug mode enabled"
        KOEBRIDGE_DEBUG=1 ./build/koebridge
    else
        ./build/koebridge
    fi
fi