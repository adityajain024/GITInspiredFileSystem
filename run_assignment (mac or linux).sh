#!/bin/bash

# Get the absolute path of this script
SCRIPT_DIR="$(cd "$(dirname "$0")" && pwd)"

# Navigate to the script directory
cd "$SCRIPT_DIR"

# Compile the C++ file
g++ assignment.cpp -o assignment

# Check if compilation succeeded
if [ $? -eq 0 ]; then
    echo "Compilation successful! Running program..."
    echo "Press Ctrl+D (or Ctrl+C) to exit when finished."
    # Open a new terminal and run the program
    if [[ "$OSTYPE" == "darwin"* ]]; then
        # macOS: open Terminal.app and run
        osascript -e "tell application \"Terminal\" to do script \"cd '$SCRIPT_DIR'; ./assignment\""
    else
        # Linux (common desktop environments)
        if command -v gnome-terminal &> /dev/null; then
            gnome-terminal -- bash -c "cd '$SCRIPT_DIR'; ./assignment; exec bash"
        elif command -v xterm &> /dev/null; then
            xterm -hold -e "cd '$SCRIPT_DIR'; ./assignment"
        else
            echo "Could not detect terminal emulator. Run './assignment' manually."
        fi
    fi
else
    echo "Compilation failed."
fi
