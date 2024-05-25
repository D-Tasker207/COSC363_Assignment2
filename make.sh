#!/bin/bash

# Get the script directory
base_dir=$(dirname "$0")
build_dir="$base_dir/build"

# Check for --clean argument
if [ "$1" == "--clean" ] ; then
    bash "$base_dir/clean.sh" -y
fi

# Build the project
echo "Building Project..."
mkdir -p "$build_dir"
cd "$build_dir"
cmake ".."
if [ $? -ne 0 ]; then # Check for errors
    echo "CMake configuration failed"
    exit 1
fi
cmake --build "."
if [ $? -ne 0 ]; then # Check for errors
    echo "CMake build failed"
    exit 1
fi
echo "Build complete!"