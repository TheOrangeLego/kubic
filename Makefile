# compilers
CPP_COMPILER = g++
ASM_COMPILER = nasm

# generated binaries
COMPILER = kubicc
DRIVER   = main

# flags for g++ compiler
CF_OBJECT = -c
CF_OUTPUT = -o
CF_DEBUG  = -ggdb
CF_ERRORS = -Wall -Wextra -Wsign-conversion
CF_HEADER_DIR = -I ./

# flags for nasm compiler
AF_L64   = -f elf64
AF_DEBUG = -g

# headers
COMPILER_HEADERS = compiler/*.hpp
PARSER_HEADERS   = parser/*.hpp
SHARED_HEADERS   = shared/*.hpp

# kubic compiler and driver source and generated objects
KUBIC_COMPILER_SOURCE = kubicc.cpp
KUBIC_COMPILER_OBJECT = kubicc.o
KUBIC_DRIVER_SOURCE   = driver.cpp
KUBIC_DRIVER_OBJECT   = driver.o

# generated kubic asm and object files
KUBIC_GENERATED_ASM    = main.ka
KUBIC_GENERATED_OBJECT = main.o

compiler: $(COMPILER_HEADERS) $(PARSER_HEADERS) $(SHARED_HEADERS) $(KUBIC_COMPILER_SOURCE)
	$(CPP_COMPILER) $(CF_ERRORS) $(CF_DEBUG) $(CF_OBJECT) $(CF_HEADER_DIR) $(KUBIC_COMPILER_SOURCE)
	$(CPP_COMPILER) $(KUBIC_COMPILER_OBJECT) $(CF_OUTPUT) $(COMPILER)

driver:
	$(ASM_COMPILER) $(AF_L64) $(AF_DEBUG) $(AF_OUTPUT) $(KUBIC_GENERATED_ASM)
	$(CPP_COMPILER) $(CF_ERRORS) $(CF_DEBUG) $(CF_OBJECT) $(KUBIC_DRIVER_SOURCE)
	$(CPP_COMPILER) $(CF_OUTPUT) $(DRIVER) $(KUBIC_DRIVER_OBJECT) $(KUBIC_GENERATED_OBJECT)

clean:
	rm -f $(KUBIC_COMPILER_OBJECT) $(KUBIC_DRIVER_OBJECT) $(KUBIC_GENERATED_ASM) $(KUBIC_GENERATED_OBJECT)
	rm -f $(COMPILER) $(DRIVER)
