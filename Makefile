# Makefile for compiling and linking main.c, cpu.c, and microui.c with SDL2

CC = gcc
LDFLAGS = `sdl2-config --cflags --libs -ggdb`

# Source files
SRC = src/main.c src/cpu.c

# Object files (generated from the source files)
OBJ = $(SRC:.c=.o)

# Output executable
OUT = bin/chip8

# Default target
all: $(OUT)

# Rule to compile source files
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# Rule to link the object files into the final executable
$(OUT): $(OBJ)
	# Ensure bin/ directory exists
	mkdir -p bin
	$(CC) $(OBJ) -o $(OUT) $(LDFLAGS)

# Clean up object files and executable
clean:
	rm -f $(OBJ) $(OUT)

# Rebuild the project
rebuild: clean all

