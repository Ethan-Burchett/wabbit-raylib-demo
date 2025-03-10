#!/bin/bash

# Change to your C project directory
cd /Users/eth/dev/GameDev/wabbit-raylib-demo || exit 1

# Run a few Bash commands
echo "Compiling the project..."
make clean && make

echo "Running the program..."
./bin/Debug/wabbit-raylib-demo