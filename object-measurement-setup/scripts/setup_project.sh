#!/bin/bash

# Detect the operating system
OS="$(uname -s)"

# Create a build directory if it doesn't exist
if [ ! -d "build" ]; then
    mkdir build
fi

# Navigate to the build directory
cd build

# Run CMake to configure the project
case "$OS" in
    Linux|Darwin)
        cmake ..
        ;;
    CYGWIN*|MINGW32*|MSYS*|MINGW*)
        cmake -G "MinGW Makefiles" ..
        ;;
    *)
        echo "Unsupported operating system: $OS"
        exit 1
        ;;
esac

# Build the project
case "$OS" in
    Linux|Darwin)
        make
        ;;
    CYGWIN*|MINGW32*|MSYS*|MINGW*)
        mingw32-make
        ;;
    *)
        echo "Unsupported operating system: $OS"
        exit 1
        ;;
esac

# Print a message indicating that the setup is complete
echo "Project setup complete. You can now build and run the application."