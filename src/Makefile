# Makefile for Tetris

CC = gcc
CFLAGS = -std=c11 -D_POSIX_C_SOURCE=199309L -fprofile-arcs -ftest-coverage
LDFLAGS = -lncurses

SRC_DIR = ../src/brick_game/tetris
OBJ = tetris.o main.o
TARGET = tetris

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJ) $(LDFLAGS)

tetris.o: $(SRC_DIR)/tetris.c
	$(CC) $(CFLAGS) -c $(SRC_DIR)/tetris.c

main.o: $(SRC_DIR)/main.c
	$(CC) $(CFLAGS) -c $(SRC_DIR)/main.c

clean:
	rm -f $(OBJ) $(TARGET) *.gcda *.gcno tetris-tetris.gcno
