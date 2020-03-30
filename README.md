# Kubic

Kubic is an open source programming language designed to be simple and accessible for those new to Computer Science or
software programming, and a non-verbose language of choice for those familiar with the field, similar to the likes of
Python.

To be honest, there are no standout features in Kubic that you could not find in other languages, nor is it blazing
fast. Instead, I see this project as a toy language that welcomes all to use for any purpose they desire.

### Sample Code

Below are different sample Kubic code snippets with their intended purpose.

```
```

### Compiling from Source

##### Requirements

In order to compile this project from source, the following dependencies are required and expected to be met

* an AMD64 executable CPU
* C++ Boost library

##### Process

Before we begin compiling the project, we need to understand the build process. There are currently four primary
pipelines that compose the process of compiling a Kubic `*.kbc` file to generating an executable binary file.

1.  `*.kbc` files are parsed and tokens are generated with additional metadata

2.  An abstract syntax tree is composed from said tokens

3.  AMD64 assembly code is generated as the abstract syntax tree is traversed through

4.  A binary executable is created by bundling the assembly file with a Cpp binary file

The above pipelines are grouped into two `make` lanes, pipelines 1, 2, and 3 forming the Kubic compiler,
and pipeline 4 forming the Kubic driver. As this disjoint can be rather odd and will impede users from having a smooth
and continuous workflow, there are goals of having the role of the driver be part of the compiler too.

Now that we have explored the process of compiling Kubic programs, the following are the commands necessary to
generate a Kubic executable binary:

1.  `make compiler` -  generate the Kubic compiler, named `kubic`

2.  `kubic foo.kbc` - compiles `foo.kbc`, generating a `kubic.asm` assembly file

3.  `make driver` - creates a binary executable from the assembly file mentioned above

##### File Structure

```
kubic/
├── compiler/
│   ├── assembly.hpp
│   ├── compiler.hpp
│   └── messages.hpp
├── parser/
│   ├── messages.hpp
│   ├── lexer.hpp
│   ├── parser.hpp
│   └── rules.hpp
├── shared/
│   ├── helpers.hpp
│   ├── nodes.hpp
│   └── tokenss.hpp
├── kubic.cpp
├── Makefile
└── README.md
```
