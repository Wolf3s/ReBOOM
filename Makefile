SRC = $(wildcard *.c)
OBJ = $(SRC:.c=.o)
CC = gcc
CFLAGS = -Wall -DUNIX -g -DINSTRUMENTED -DRANGECHECK
LDFLAGS = -lSDL2 -lSDL2_mixer -lSDL2_net -lm

reboom: $(OBJ)
	$(CC) -o $@ $^ $(LDFLAGS)

.PHONY: clean
clean:
	rm -f $(OBJ) reboom
