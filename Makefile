COMPILER = g++
BINARY   = kubic

# flags for g++ compiler
F_OBJECT = -c
F_OUTPUT = -o
F_ERRORS = -Wall -Wextra -Wsign-conversion

HEADER_SOURCE = headers/**/*.cpp
HEADER_PRE_CH = headers/**/*.hpp.gch
OBJECT_FILES  = *.o

build:
	$(COMPILER) $(F_ERRORS) $(F_OBJECT) $(HEADER_SOURCE) main.cpp
	$(COMPILER) $(OBJECT_FILES) $(F_OUTPUT) $(BINARY)

clean:
	$(RM) $(HEADER_PRE_CH) $(OBJECT_FILES) $(BINARY)
