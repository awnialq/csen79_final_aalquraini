#!/usr/bin/env python3
"""
Script to compile and run the decisionTree program.
Usage: python run_decision_tree.py <text_file>
"""

import subprocess
import sys
import os


def main():
    if len(sys.argv) != 2:
        print(f"Usage: {sys.argv[0]} <text_file>")
        sys.exit(1)

    text_file = sys.argv[1]

    # Get the directory where this script is located
    script_dir = os.path.dirname(os.path.abspath(__file__))

    # Change to the script directory to run make
    os.chdir(script_dir)

    # Compile the program using make
    print("Compiling decisionTree...")
    compile_result = subprocess.run(["make", "clean", "decisionTree"], capture_output=True, text=True)

    if compile_result.returncode != 0:
        print("Compilation failed!")
        print(compile_result.stderr)
        sys.exit(1)

    print("Compilation successful!")
    print()

    # Run the decisionTree program with the text file passed twice
    print(f"Running: ./decisionTree {text_file} {text_file}")
    print("-" * 50)

    run_result = subprocess.run(
        ["./decisionTree", text_file, text_file],
        capture_output=False
    )

    sys.exit(run_result.returncode)


if __name__ == "__main__":
    main()
