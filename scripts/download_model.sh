#!/bin/bash

# Default values
model_path="."  # Changed to current directory
model_version="1.0"

# Function to display usage information
usage() {
    echo "Usage: $0 [OPTIONS] MODEL_ID"
    echo
    echo "Download a translation or speech-to-text model for KoeBridge."
    echo
    echo "Options:"
    echo "  -h, --help                  Display this help message"
    echo "  -l, --list                  List available models"
    echo "  -p, --path PATH             Specify model download path (default: current directory)"
    echo "  -v, --version VERSION       Specify model version (default: $model_version)"
    echo
    echo "Available models:"
    echo "Translation Models:"
    echo "  nllb-ja-en     NLLB-200 Distilled 600M model optimized for Japanese to English"
    echo "  nllb-en-ja     NLLB-200 Distilled 600M model optimized for English to Japanese"
    echo "  nllb-zh-en     NLLB-200 Distilled 600M model optimized for Chinese to English"
    echo "  nllb-en-zh     NLLB-200 Distilled 600M model optimized for English to Chinese"
    echo "  nllb-multi     NLLB-200 Distilled 600M model with support for multiple languages"
    echo
    echo "Speech-to-Text Models:"
    echo "  whisper-tiny   Whisper tiny model (fastest, lowest accuracy)"
    echo "  whisper-base   Whisper base model (fast, good accuracy)"
    echo "  whisper-small  Whisper small model (balanced speed/accuracy)"
    echo "  whisper-medium Whisper medium model (slower, high accuracy)"
    echo "  whisper-large  Whisper large model (slowest, highest accuracy)"
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
            echo "Translation Models:"
            echo "  nllb-ja-en     NLLB-200 Distilled 600M model optimized for Japanese to English"
            echo "  nllb-en-ja     NLLB-200 Distilled 600M model optimized for English to Japanese"
            echo "  nllb-zh-en     NLLB-200 Distilled 600M model optimized for Chinese to English"
            echo "  nllb-en-zh     NLLB-200 Distilled 600M model optimized for English to Chinese"
            echo "  nllb-multi     NLLB-200 Distilled 600M model with support for multiple languages"
            echo
            echo "Speech-to-Text Models:"
            echo "  whisper-tiny   Whisper tiny model (fastest, lowest accuracy)"
            echo "  whisper-base   Whisper base model (fast, good accuracy)"
            echo "  whisper-small  Whisper small model (balanced speed/accuracy)"
            echo "  whisper-medium Whisper medium model (slower, high accuracy)"
            echo "  whisper-large  Whisper large model (slowest, highest accuracy)"
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

# Base URLs for models
NLLB_BASE_URL="https://huggingface.co/facebook/nllb-200-distilled-600M/resolve/main"
WHISPER_BASE_URL="https://openaipublic.azureedge.net/main/whisper/models"

# Download the model
case "$model_id" in
    # NLLB Translation Models
    nllb-ja-en)
        echo "Downloading NLLB-200 Distilled 600M model (Japanese to English)..."
        curl -L "$NLLB_BASE_URL/pytorch_model.bin" -o "$model_path/nllb-200-distilled-600m-ja-en.bin"
        ;;
    nllb-en-ja)
        echo "Downloading NLLB-200 Distilled 600M model (English to Japanese)..."
        curl -L "$NLLB_BASE_URL/pytorch_model.bin" -o "$model_path/nllb-200-distilled-600m-en-ja.bin"
        ;;
    nllb-zh-en)
        echo "Downloading NLLB-200 Distilled 600M model (Chinese to English)..."
        curl -L "$NLLB_BASE_URL/pytorch_model.bin" -o "$model_path/nllb-200-distilled-600m-zh-en.bin"
        ;;
    nllb-en-zh)
        echo "Downloading NLLB-200 Distilled 600M model (English to Chinese)..."
        curl -L "$NLLB_BASE_URL/pytorch_model.bin" -o "$model_path/nllb-200-distilled-600m-en-zh.bin"
        ;;
    nllb-multi)
        echo "Downloading NLLB-200 Distilled 600M model (Multilingual)..."
        curl -L "$NLLB_BASE_URL/pytorch_model.bin" -o "$model_path/nllb-200-distilled-600m-multi.bin"
        ;;
    # Whisper Speech-to-Text Models
    whisper-tiny)
        echo "Downloading Whisper tiny model..."
        curl -L "$WHISPER_BASE_URL/81f7c96c852ee8f83232b23b7745d49ac99d36fa88aa4a715365ff6258cd4c8b9.pt" -o "$model_path/whisper-tiny.pt"
        ;;
    whisper-base)
        echo "Downloading Whisper base model..."
        curl -L "$WHISPER_BASE_URL/ed3a89bda2dceb8ba95e9d29e1dce6eb5c1f1b2e9b3f3e1e5e1a74020b06f22.pt" -o "$model_path/whisper-base.pt"
        ;;
    whisper-small)
        echo "Downloading Whisper small model..."
        curl -L "$WHISPER_BASE_URL/9ecf779ab21c1c3035ac893221dd789f9f410f13e26118e0f965a81ee07e1c47.pt" -o "$model_path/whisper-small.pt"
        ;;
    whisper-medium)
        echo "Downloading Whisper medium model..."
        curl -L "$WHISPER_BASE_URL/345ae4da62f9b3d59415afc1bc0dd4079a8562e966d78e86c6811f770249e188.pt" -o "$model_path/whisper-medium.pt"
        ;;
    whisper-large)
        echo "Downloading Whisper large model..."
        curl -L "$WHISPER_BASE_URL/81f7c96c852ee8f83232b23b7745d49ac99d36fa88aa4a715365ff6258cd4c8b9.pt" -o "$model_path/whisper-large.pt"
        ;;
    *)
        echo "Error: Unknown model ID: $model_id"
        usage
        ;;
esac

# Check if download was successful
if [ $? -eq 0 ]; then
    echo "Model downloaded successfully to $model_path"
else
    echo "Error: Failed to download model"
    exit 1
fi 