#!/bin/bash

# Function to display usage information
show_usage() {
    echo "Usage: $0 [OPTIONS] MODEL_ID"
    echo "Download a translation model for KoeBridge"
    echo ""
    echo "Options:"
    echo "  -h, --help     Display this help message"
    echo "  -l, --list     List available models"
    echo "  -p, --path     Specify model download path (default: ./_dataset/models)"
    echo "  -v, --version  Specify model version (default: latest)"
    echo ""
    echo "Example:"
    echo "  $0 -p /path/to/models nllb-ja-en"
    echo "  $0 --version 1.0.0 nllb-ja-en"
}

# Default values
MODEL_PATH="../_dataset/models"
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
            echo "  nllb-ja-en - NLLB-200 Distilled 600M (Japanese to English)"
            echo "    - Optimized for Apple Silicon"
            echo "    - Quantized for efficient inference"
            echo "    - Size: ~1.2GB"
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

# Download model based on ID
case "$MODEL_ID" in
    "nllb-ja-en")
        echo "Downloading NLLB-200 Distilled 600M (Japanese to English) model..."
        MODEL_URL="https://huggingface.co/TheBloke/NLLB-200-Distilled-600M-GGML/resolve/main/nllb-200-distilled-600m-ja-en-q4_K_M.bin"
        MODEL_FILE="nllb-200-distilled-600m-ja-en.bin"
        ;;
    *)
        echo "Error: Unknown model ID: $MODEL_ID"
        echo "Use --list to see available models"
        exit 1
        ;;
esac

# Download the model
echo "Downloading to: $MODEL_PATH/$MODEL_FILE"
curl -L "$MODEL_URL" -o "$MODEL_PATH/$MODEL_FILE"

# Check if download was successful
if [ $? -eq 0 ]; then
    echo "Download completed successfully!"
    echo "Model saved to: $MODEL_PATH/$MODEL_FILE"
else
    echo "Error: Failed to download model"
    exit 1
fi 