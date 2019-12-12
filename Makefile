CPP_COMPILER = g++
ASM_COMPILER = nasm

# generated binaries
COMPILER = kubic-c
DRIVER   = main

# flags for g++ compiler
CF_OBJECT = -c
CF_OUTPUT = -o
CF_ERRORS = -Wall -Wextra -Wsign-conversion

# flags for nasm compiler
AF_L64 = -f elf64
AF_L32 = -f elf32

COMPILER_SOURCE = headers/**/*.cpp
COMPILER_PRE_CH = headers/**/*.hpp.gch
COMPILER_OBJECTS = $(subst driver.o, , *.o)

compiler:
	$(CPP_COMPILER) $(CF_ERRORS) $(CF_OBJECT) $(COMPILER_SOURCE) compiler.cpp
	$(CPP_COMPILER) $(COMPILER_OBJECTS) $(CF_OUTPUT) $(COMPILER)

driver:
	$(ASM_COMPILER) $(AF_L64) $(AF_OUTPUT) main.asm
	$(CPP_COMPILER) $(CF_ERRORS) $(CF_OBJECT) driver.cpp
	$(CPP_COMPILER) $(CF_OUTPUT) $(DRIVER) driver.o main.o

clean-compiler:
	$(RM) $(COMPILER_PRE_CH) $(COMPILER_OBJECTS)

clean-driver:
	$(RM) driver.o kubic.o

clean: clean-compiler clean-driver
	$(RM) $(COMPILER) $(DRIVER)
