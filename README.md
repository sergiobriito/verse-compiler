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

- C++ Compiler (e.g., g++) and NASM (Netwide Assembler):
  - Linux:
    ```
    sudo apt update
    sudo apt install g++
    sudo apt install g++-multilib
    sudo apt install nasm
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
The executable versec will be generated in the build directory.

## Usage
To compile a Verse code file (example.vs):
```
chmod 700 ./run.sh
./run.sh example.vs
```
## Examples

- Declaration of variable:
```
let n = 100;
let compilerName = "versecompiler";
```

- For loop:
```
let i;
for (i=0;i<100;i++){
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
