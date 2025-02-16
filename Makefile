
CC = gcc
LDFLAGS = `sdl2-config --cflags --libs -ggdb`

SRC = src/main.c src/cpu.c

OBJ = $(SRC:.c=.o)

OUT = bin/chip8

all: $(OUT)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# Rule to link the object files into the final executable
$(OUT): $(OBJ)
	mkdir -p bin
	$(CC) $(OBJ) -o $(OUT) $(LDFLAGS)

# Clean up object files and executable
clean:
	rm -f $(OBJ) $(OUT)

# Rebuild the project
rebuild: clean all

