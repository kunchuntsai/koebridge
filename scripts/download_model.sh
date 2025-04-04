#!/bin/bash

# Function to display usage information
show_usage() {
    echo "Usage: $0 [OPTIONS] MODEL_ID"
    echo "Download a translation model for KoeBridge"
    echo ""
    echo "Options:"
    echo "  -h, --help     Display this help message"
    echo "  -l, --list     List available models"
    echo "  -p, --path     Specify model download path (default: ./models)"
    echo "  -v, --version  Specify model version (default: latest)"
    echo ""
    echo "Example:"
    echo "  $0 -p /path/to/models model1"
    echo "  $0 --version 1.0.0 model2"
}

# Default values
MODEL_PATH="./models"
MODEL_VERSION="latest"

# Parse command line arguments
while [[ $# -gt 0 ]]; do
    case "$1" in
        -h|--help)
            show_usage
            exit 0
            ;;
        -l|--list)
            echo "Available models:"
            echo "  model1 - Japanese to English (small)"
            echo "  model2 - Japanese to English (medium)"
            echo "  model3 - Japanese to English (large)"
            exit 0
            ;;
        -p|--path)
            MODEL_PATH="$2"
            shift 2
            ;;
        -v|--version)
            MODEL_VERSION="$2"
            shift 2
            ;;
        *)
            MODEL_ID="$1"
            shift
            ;;
    esac
done

# Check if model ID is provided
if [ -z "$MODEL_ID" ]; then
    echo "Error: Model ID is required"
    show_usage
    exit 1
fi

# Create model directory if it doesn't exist
mkdir -p "$MODEL_PATH"

# Model URLs (replace with actual URLs)
declare -A MODEL_URLS=(
    ["model1"]="https://example.com/models/model1_v1.0.bin"
    ["model2"]="https://example.com/models/model2_v1.0.bin"
    ["model3"]="https://example.com/models/model3_v1.0.bin"
)

# Check if model exists
if [ -z "${MODEL_URLS[$MODEL_ID]}" ]; then
    echo "Error: Unknown model ID: $MODEL_ID"
    echo "Use --list to see available models"
    exit 1
fi

# Download URL
DOWNLOAD_URL="${MODEL_URLS[$MODEL_ID]}"
OUTPUT_FILE="$MODEL_PATH/${MODEL_ID}.bin"

# Check if file already exists
if [ -f "$OUTPUT_FILE" ]; then
    echo "Model already exists at: $OUTPUT_FILE"
    read -p "Do you want to overwrite it? (y/N) " -n 1 -r
    echo
    if [[ ! $REPLY =~ ^[Yy]$ ]]; then
        echo "Download cancelled"
        exit 0
    fi
fi

# Download the model
echo "Downloading model $MODEL_ID (version: $MODEL_VERSION)..."
if command -v curl &> /dev/null; then
    curl -L --progress-bar "$DOWNLOAD_URL" -o "$OUTPUT_FILE"
elif command -v wget &> /dev/null; then
    wget --progress=bar:force "$DOWNLOAD_URL" -O "$OUTPUT_FILE"
else
    echo "Error: Neither curl nor wget is installed"
    exit 1
fi

# Check if download was successful
if [ $? -eq 0 ] && [ -f "$OUTPUT_FILE" ]; then
    echo "Download completed successfully"
    echo "Model saved to: $OUTPUT_FILE"
else
    echo "Error: Download failed"
    exit 1
fi 