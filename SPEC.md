# C2ASM Compiler Specification

## Project Overview
- **Name**: C2ASM Compiler
- **Type**: Native C++ compiler
- **Core**: Lexer → Parser (AST) → Code Generator (x86 NASM)
- **Language**: Simplified C-like
- **Output**: x86 assembly (NASM syntax)

## Language Features

### Data Types
- `int` - 32-bit integer
- `float` - 32-bit float
- `char` - 8-bit character

### Operators
- Arithmetic: `+`, `-`, `*`, `/`, `%`
- Comparison: `==`, `!=`, `<`, `>`, `<=`, `>=`
- Logical: `&&`, `||`, `!`
- Assignment: `=`, `+=`, `-=`, `*=`, `/=`

### Statements
- Variable declarations: `int x;`, `int x = 5;`
- Expression statements (with semicolons)
- Compound statements: `{ stmt1; stmt2; }`
- If-else: `if (cond) stmt; [else stmt;]`
- While loops: `while (cond) stmt;`
- For loops: `for (init; cond; iter) stmt;`
- Return: `return expr;`

### Functions
- Declaration: `int foo(int a, int b) { ... }`
- Main function: `void main() { ... }`
- Built-in: `print(expr);`, `read(var);`

## Architecture

### Lexer (Tokenizer)
- Input: source code string
- Output: stream of tokens
- Token types: IDENT, NUMBER, STRING, KEYWORD, OPERATOR, PUNCTUATOR

### Parser (AST Builder)
- Recursive descent parser
- Builds Abstract Syntax Tree
- Nodes: Program, Function, Declaration, If, While, For, Return, BinaryOp, UnaryOp, Assign, Call

### Code Generator
- Input: AST
- Output: x86 NASM assembly
- Register allocation: eax, ebx, ecx, edx
- Stack for overflow
- Calling convention: cdecl

## File Structure
```
include/
  - Token.h
  - AST.h
  - Lexer.h
  - Parser.h
  - Codegen.h
src/
  - main.cpp
  - Lexer.cpp
  - Parser.cpp
  - Codegen.cpp
test/
  - test1.c
```

## Build
```bash
g++ -std=c++17 -o compiler src/*.cpp
```

## Usage
```bash
./compiler input.c [-o output.asm]
```