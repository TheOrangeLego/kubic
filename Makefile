# compilers
CPP_COMPILER = g++
ASM_COMPILER = nasm

# generated binaries
COMPILER = kubicc

# flags for g++ compiler
CF_OBJECT = -c
CF_OUTPUT = -o
CF_DEBUG  = --ggdb
CF_ERRORS = -Wall -Wextra -Wsign-conversion

# flags for nasm compiler
AF_L64   = -f elf64
AF_DEBUG = -g

