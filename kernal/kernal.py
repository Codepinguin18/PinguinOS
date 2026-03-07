#!/usr/bin/env python3
import sys
import subprocess
import os

def run_command(cmd):
    print(f"Running: {cmd}")
    result = subprocess.run(cmd, shell=True)
    if result.returncode != 0:
        print(f"Error: Command failed with exit code {result.returncode}")
        sys.exit(result.returncode)

def main():
    if len(sys.argv) < 2:
        print("Usage: python3 kernal.py [build|iso|run|clean]")
        sys.exit(1)

    action = sys.argv[1].lower()
    
    base_make_cmd = "make CROSS=i686-linux-gnu"

    if action == "build":
        run_command(f"{base_make_cmd} all")
    elif action == "iso":
        run_command(f"{base_make_cmd} iso")
    elif action == "run":
        run_command(f"{base_make_cmd} run")
    elif action == "clean":
        run_command(f"{base_make_cmd} clean")
    else:
        print(f"Unknown action: {action}")
        print("Usage: python3 kernal.py [build|iso|run|clean]")
        sys.exit(1)

if __name__ == "__main__":
    main()
