CC      = gcc
CFLAGS  = -Wall -Wextra -std=c11 -g -Ilibs -Isrc

SRC     = $(wildcard libs/*.c) $(wildcard src/*.c)
OBJ     = $(SRC:.c=.o)
TARGET  = raiden

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CC) $(CFLAGS) -o $@ $^

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

run: all
	./$(TARGET)

clean:
	rm -f $(OBJ) $(TARGET)

.PHONY: all run clean
