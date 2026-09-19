# Verse Compiler

Verse Compiler is a small C++ compiler for the Verse language. It tokenizes
source code, builds an AST, and emits x86-64 NASM assembly.

The compiler design notes are available in the
[Compiler Design repository](https://github.com/sergiobriito/compiler-design).

## Supported Features

- Integer variables and string variables
- Variable declarations with `let`
- Assignment with `=`
- Arithmetic operators: `+`, `-`, `*`, `/`
- Comparisons: `==`, `!=`, `<`, `<=`, `>`, `>=`
- `if`/`else` statements
- `for` loops with `++` and `--`
- Printing variables with `print(...)`
- Integer arrays with indexed reads
- Integer functions with parameters, `return`, and recursive calls
- Variables declared inside a `for` or `if` block are scoped to that block

## Requirements

On Ubuntu or Debian, install the native compiler, NASM, and GCC:

```bash
sudo apt update
sudo apt install g++ nasm
```

The generated assembly targets native x86-64 Linux.

## Build

Clone the repository and build the compiler:

```bash
git clone https://github.com/sergiobriito/verse-compiler.git
cd verse-compiler
make
```

This creates the `versec` executable in the repository root.

## Run

The helper script compiles, assembles, links, runs, and then removes its
temporary output files:

```bash
./run.sh example.vs
```

## Example

The main example is in [example.vs](example.vs):

```verse
fn fibonacci(n){
	if (n <= 2){
		return 1;
	};
	return fibonacci(n - 2) + fibonacci(n - 1);
};

let n = fibonacci(10);
print(n);
```

Run it with:

```bash
./run.sh example.vs
```

Expected output:

```text
55
```

## Arrays

Arrays contain integers and use zero-based indexing:

```verse
let values = [10, 20, 30];
let index = 1;
print(values[index]);
```

Run the focused example with:

```bash
./run.sh examples/arrays.vs
```

Expected output:

```text
20
```

Array indexes must be valid integers. 
Out-of-bounds access is reported by the compiler.