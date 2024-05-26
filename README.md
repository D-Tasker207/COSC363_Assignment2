# Computer Graphics Assignment 2

## Description
Ray tracing project creating a demo scene and showing off some ray tracing features. See `<PDF NAME HERE>` for full report.
## Usage
To build the project run `make.sh` to create the `/build` an `/bin` directories. Find the executable in the `/bin` directory

## Scripts
### `clean.sh`
Cleans the build and bin directories of all files  
    -y skips the confirmation for file deletion

### `make.sh`
Builds the project using CMake files  
    --clean cleans the build and bin directories before building
    --release builds project with compiler optimizations enabled significantly improving render times
    --debug builds project with debug flag and address sanitization for more clear memory stack traces
