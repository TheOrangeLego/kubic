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

# headers
COMPILER_HEADERS = compiler/*.hpp
PARSER_HEADERS   = parser/*.hpp
SHARED_HEADERS   = shared/*.hpp

# kubic compiler source
KUBIC_COMPILER_SOURCE = kubicc.cpp

compiler: $(COMPILER_HEADERS) $(PARSER_HEADERS) $(SHARED_HEADERS) 
	$(CPP_COMPILER) $(CF_ERRORS) $(CF_DEBUG) $(CF_OBJECT) $(KUBIC_COMPILER_SOURCE)
