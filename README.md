# Verse Compiler

## Introduction

It is a C++ compiler designed to compile 
programs written in a custom programming language 
called Verse into 32-bit x86 assembly code.

This compiler adheres to the principles of compiler design, 
which include tokenization, constructing an 
abstract syntax tree (AST), and generating code. 

I have documented my Compiler Design analyses in the following repository:
- [Compiler Design](https://github.com/sergiobriito/compiler-design)

## Features

- **Verse Language**: A custom programming language with basics 
features, including variable declaration, assignment, arithmetic operations, conditionals, and loops.
- **Compiler**: Converts Verse code (.vs) into 32-bit x86 assembly code.

## Prerequisites

You need the following tools installed on your system:

- C++ Compiler (e.g., g++):
  - Linux:
    ```
    sudo apt-get update
    sudo apt-get install g++
    ```
  - Installation of C++ 32-bit libs:
    ```
    sudo apt install g++-multilib
    ```
    
- NASM (Netwide Assembler):
  - Linux:
    ```
    sudo apt-get install nasm
    ```

## Installation

1. Clone this repository:
```
git clone https://github.com/sergiobriito/verse-compiler.git
cd verse-compiler
```
1. Build the compiler:
```
make 
```
The executable verse-compiler will be generated in the build directory.

## Usage
To compile a Verse code file (code.vs) and generate the assembly output:

This will generate an output file named out.asm containing the 32-bit x86 assembly code.

To compile the out.asm code file and generate the output:
```
chmod 700 ./nasm-script.sh
./nasm-script.sh
```
## Examples of Verse Language

- Declaration of variable:
```
let n = 100;
let compilerName = "versecompiler";
```

- For loop:
```
let i;
for (i=0;i<10;i++){
print(i);
};
```

- If:
```
let k = 100;
if (k > 99){
print(k);
};
```