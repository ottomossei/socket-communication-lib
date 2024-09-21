#!/bin/bash

if [ -d "build" ]; then
    echo "Removing existing build directory..."
    rm -r build
fi

mkdir build
cd build
cmake .. -G Ninja

# build
if ninja all; then
    echo "Build succeeded."
    echo "Running server_example..."
    ./server_example
else
    echo "Build failed. Please check the error messages."
    exit 1
fi
