#!/bin/bash

# Get the script directory
base_dir=$(dirname "$0")
build_dir="$base_dir/build"

# Function to confirm deletion
confirm_deletion() {
  read -r -p "The listed files will be permenantly deleted. Are you sure you want to continue? (y/N) " response
  case "$response" in
    [yY]*)
      echo "Confirmed! Proceeding with deletion..."
      return 0
      ;;
    *)
      echo "Deletion cancelled."
      return 1
      ;;
  esac
}

# Preview files with find and print
echo "Files to be deleted:"
find "$build_dir" -mindepth 1 -print

# Check for -y argument for skipping confirmation
if [[ "$1" == "-y" ]]; then
  echo "Cleaning build dir..."
else
  # Ask for confirmation
  if ! confirm_deletion; then
    exit 1
  fi
fi

# Delete files
find "$build_dir" -mindepth 1 -delete
echo "Deletion complete!"