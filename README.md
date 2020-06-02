# Kubic

Kubic is an open source programming language meant to be consice and fast.

### Motivation

Coming from a background in static type languages, I appreciated the simplistic syntax from Python, though I was never
a fan of its dynamic type system. Instead of relying on additional frameworks or libraries, Kubic at its core is a
static type, non-verbose programming language.

### Sample Code

### Compiling from Source

##### Requirements

* AMD64 executable CPU
* C++ Boost library

##### Process

There are currently four primary pipelines that form the process of compiling a Kubic `*.kbc` source file to
generate an executable binary file.

1.  `*.kbc` files are parsed and tokens are generated with additional metadata

2.  An abstract syntax tree is composed from said tokens

3.  AMD64 assembly code is generated as the abstract syntax tree is traversed through

4.  A binary executable is created by bundling the assembly file with a CPP source file

### File Structure

```
kubic/
├── compiler/
│   ├── assembly.hpp
│   ├── compiler.hpp
├── parser/
│   ├── lexer.hpp
│   ├── parser.hpp
│   └── rules.hpp
├── shared/
│   ├── error.hpp
│   ├── helpers.hpp
│   ├── messages.hpp
│   ├── node.hpp
│   ├── token.hpp
│   └── types.hpp
├── driver.cpp
├── kubicc.cpp
├── Makefile
└── README.md
```
