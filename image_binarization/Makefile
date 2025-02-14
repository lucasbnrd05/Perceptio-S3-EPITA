# Makefile

CC = gcc
CFLAGS = -Wall -Wextra
LDFLAGS =
LDLIBS = -lSDL2 -lSDL2_image -lm

EXE = main
SRC = main.c binarization.c


$(EXE): $(SRC)
	$(CC) $(CFLAGS) $(SRC) -o $(EXE) $(LDLIBS)

.PHONY: clean

clean:
	${RM} $(EXE)

# END
