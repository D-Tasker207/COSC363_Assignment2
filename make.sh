#!/bin/bash

# Get the script directory
base_dir=$(dirname "$0")
build_dir="$base_dir/build"
debug_build=0
release_build=0
clean_build=0

# Check for --clean and --debug arguments
for arg in "$@"
do
    if [ "$arg" == "--clean" ] ; then
        clean_build=1
    elif [ "$arg" == "--debug" ] ; then
        debug_build=1
    elif [ "$arg" == "--release" ] ; then
        release_build=1
    fi
done

# Check for --clean argument
if [ $clean_build -eq 1 ] ; then
    bash "$base_dir/clean.sh" -y
fi

# Build the project
echo "Building Project..."
mkdir -p "$build_dir"
cd "$build_dir"
if [ $debug_build -eq 1 ] ; then
    cmake -DCMAKE_BUILD_TYPE=Debug ".."
elif [ $release_build -eq 1 ] ; then
    cmake -DCMAKE_BUILD_TYPE=Release ".."
else
    cmake ".."
fi
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