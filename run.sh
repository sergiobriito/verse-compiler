#!/bin/bash

# Check number of arguments
if [ "$#" -ne 1 ]; then
  echo "Usage: $0 <source_file>"
  exit 1
fi

# Assign argument to a variable
source_file=$1

# Check if the source file exists
if [ ! -f "$source_file" ]; then
  echo "Source file '$source_file' does not exist"
  exit 1
fi

# Function to clean up files
cleanup() {
  rm -rf out out.o out.asm
}

# Trap cleanup function on exit
trap cleanup EXIT

# Compile the source code (.vs)
if ! ./versec "$source_file"; then
  exit 1
fi

# Assemble the output
if ! nasm -f elf32 out.asm -o out.o; then
  exit 1
fi

# Link the file to create the executable
if ! gcc -m32 -o out out.o -lm -nostartfiles -no-pie; then
  exit 1
fi

# Run the executable
if ! ./out; then
  exit 1
fi
