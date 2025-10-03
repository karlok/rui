# === Makefile ===

# make        - builds the rui uemo
# ./rui_demo  - run it
# make clean  - removes the binary and object files

CC = gcc
CFLAGS = -Wall -Wextra -std=c99 -I/opt/homebrew/include
LDFLAGS = -L/opt/homebrew/lib -lraylib \
  -framework OpenGL -framework Cocoa -framework IOKit -framework CoreAudio -framework CoreVideo

SRC = src/main.c
OBJ = $(SRC:.c=.o)
TARGET = rui_demo

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CC) $(OBJ) -o $@ $(LDFLAGS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJ) $(TARGET)
