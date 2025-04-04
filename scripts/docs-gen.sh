#!/bin/bash

# Change to the project root directory
cd "$(dirname "$0")/.."

# Generate documentation using doxygen
doxygen docs/Doxyfile

# Print success message
echo "Documentation generated successfully in the docs directory" 