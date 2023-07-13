#!/bin/bash

# VSCode has a bug that GTK_PATH variable is always set, with this GTK will fail
unset GTK_PATH

# Generate build directory
mkdir -p build
cd build

# Generate the CMake output
cmake ..

# Build the app with Make
make

# Copy the executable to the .deb package
cp mkc ../mkc/usr/bin/