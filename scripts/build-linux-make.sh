#!/bin/bash

# Navigate up one directory
cd ..

# Run Premake (adjust path for Linux)
./vendor/bin/premake/Linux/premake5 gmake2

# Return to original directory
cd -

# Pause equivalent: wait for user input
read -p "Press Enter to continue..."