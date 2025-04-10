#!/bin/bash

# Script to remove trailing spaces from all relevant files in the project
# Usage: ./remove_trailing_spaces.sh

set -e  # Exit on error

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
NC='\033[0m' # No Color

echo -e "${YELLOW}Starting trailing space removal...${NC}"

# Counter for processed files
processed=0
modified=0

# Find all relevant files and process them
find . -type f \
    ! -path "*/\.*" \
    ! -path "*/build/*" \
    ! -path "*/third_party/*" \
    ! -path "*/_dataset/*" \
    \( -name "*.cc" -o -name "*.h" -o -name "*.md" -o -name "*.txt" \) \
    -print0 | while IFS= read -r -d '' file; do
    
    # Skip binary files
    if file "$file" | grep -q "binary file"; then
        continue
    fi
    
    processed=$((processed + 1))
    
    # Create a backup of the original file
    # cp "$file" "${file}.bak"
    
    # Remove trailing spaces
    if sed -i '' 's/ *$//' "$file" 2>/dev/null; then
        # Check if the file was actually modified
        if ! cmp -s "$file" "${file}.bak"; then
            modified=$((modified + 1))
            echo -e "${GREEN}Modified:${NC} $file"
        else
            rm "${file}.bak"
        fi
    else
        echo -e "${RED}Error processing:${NC} $file"
        rm "${file}.bak"
    fi
done

echo -e "\n${YELLOW}Summary:${NC}"
echo -e "Total files processed: $processed"
echo -e "Files modified: $modified"
echo -e "${GREEN}Trailing space removal completed!${NC}" 