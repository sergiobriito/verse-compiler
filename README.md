# Verse Compiler

Verse Compiler is a small C++ compiler for the Verse language. It tokenizes
source code, builds an AST, and emits 32-bit x86 NASM assembly.

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

On Ubuntu or Debian, install the native compiler, NASM, and the 32-bit GCC
toolchain:

```bash
sudo apt update
sudo apt install g++ gcc-multilib libc6-dev-i386 nasm
```

The 32-bit packages are required because the generated assembly is assembled
as `elf32` and linked with `gcc -m32`.

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

## Dynamic Programming Example

[examples/fibonacci-dp.vs](examples/fibonacci-dp.vs) computes Fibonacci
iteratively with rolling state. It uses $O(n)$ time and $O(1)$ space without
recursion or array element assignment.

```bash
./run.sh examples/fibonacci-dp.vs
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

Array indexes must be valid integers. Out-of-bounds access is reported by the
compiler. Array values are currently compile-time data; array element
assignment and arrays of strings are not supported yet.