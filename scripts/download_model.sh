#!/bin/bash

# Default values
model_path="../_dataset/models"
model_version="1.0"

# Function to display usage information
usage() {
    echo "Usage: $0 [OPTIONS] MODEL_ID"
    echo
    echo "Download a translation model for KoeBridge."
    echo
    echo "Options:"
    echo "  -h, --help                  Display this help message"
    echo "  -l, --list                  List available models"
    echo "  -p, --path PATH             Specify model download path (default: $model_path)"
    echo "  -v, --version VERSION       Specify model version (default: $model_version)"
    echo
    echo "Available models:"
    echo "  nllb-ja-en     NLLB-200 Distilled 600M model optimized for Japanese to English"
    echo "  nllb-en-ja     NLLB-200 Distilled 600M model optimized for English to Japanese"
    echo "  nllb-zh-en     NLLB-200 Distilled 600M model optimized for Chinese to English"
    echo "  nllb-en-zh     NLLB-200 Distilled 600M model optimized for English to Chinese"
    echo "  nllb-multi     NLLB-200 Distilled 600M model with support for multiple languages"
    echo
    exit 1
}

# Parse command line arguments
while [[ $# -gt 0 ]]; do
    case "$1" in
        -h|--help)
            usage
            ;;
        -l|--list)
            echo "Available models:"
            echo "  nllb-ja-en     NLLB-200 Distilled 600M model optimized for Japanese to English"
            echo "  nllb-en-ja     NLLB-200 Distilled 600M model optimized for English to Japanese"
            echo "  nllb-zh-en     NLLB-200 Distilled 600M model optimized for Chinese to English"
            echo "  nllb-en-zh     NLLB-200 Distilled 600M model optimized for English to Chinese"
            echo "  nllb-multi     NLLB-200 Distilled 600M model with support for multiple languages"
            exit 0
            ;;
        -p|--path)
            model_path="$2"
            shift 2
            ;;
        -v|--version)
            model_version="$2"
            shift 2
            ;;
        *)
            model_id="$1"
            shift
            ;;
    esac
done

# Check if model ID is provided
if [ -z "$model_id" ]; then
    echo "Error: No model ID specified."
    usage
fi

# Create model directory if it doesn't exist
mkdir -p "$model_path"

# Download the model
case "$model_id" in
    nllb-ja-en)
        echo "Downloading NLLB-200 Distilled 600M model (Japanese to English)..."
        curl -L "https://huggingface.co/TheBloke/nllb-200-distilled-600M-GGML/resolve/main/nllb-200-distilled-600M.q4_0.bin" -o "$model_path/nllb-ja-en.bin"
        ;;
    nllb-en-ja)
        echo "Downloading NLLB-200 Distilled 600M model (English to Japanese)..."
        curl -L "https://huggingface.co/TheBloke/nllb-200-distilled-600M-GGML/resolve/main/nllb-200-distilled-600M.q4_0.bin" -o "$model_path/nllb-en-ja.bin"
        ;;
    nllb-zh-en)
        echo "Downloading NLLB-200 Distilled 600M model (Chinese to English)..."
        curl -L "https://huggingface.co/TheBloke/nllb-200-distilled-600M-GGML/resolve/main/nllb-200-distilled-600M.q4_0.bin" -o "$model_path/nllb-zh-en.bin"
        ;;
    nllb-en-zh)
        echo "Downloading NLLB-200 Distilled 600M model (English to Chinese)..."
        curl -L "https://huggingface.co/TheBloke/nllb-200-distilled-600M-GGML/resolve/main/nllb-200-distilled-600M.q4_0.bin" -o "$model_path/nllb-en-zh.bin"
        ;;
    nllb-multi)
        echo "Downloading NLLB-200 Distilled 600M model (Multilingual)..."
        curl -L "https://huggingface.co/TheBloke/nllb-200-distilled-600M-GGML/resolve/main/nllb-200-distilled-600M.q4_0.bin" -o "$model_path/nllb-multi.bin"
        ;;
    *)
        echo "Error: Unknown model ID: $model_id"
        usage
        ;;
esac

# Check if download was successful
if [ $? -eq 0 ]; then
    echo "Model downloaded successfully to $model_path/$model_id.bin"
    echo "You can now use this model with KoeBridge."
else
    echo "Error: Failed to download model."
    exit 1
fi 