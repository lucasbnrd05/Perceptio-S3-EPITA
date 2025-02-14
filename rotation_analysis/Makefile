CC = gcc
CFLAGS = -Wall -Wextra
LIBS = -lSDL2 -lSDL2_image -lm
TARGET = main
SOURCES = main.c manual_grid_rotation.c
OBJECTS = $(SOURCES:.c=.o)

$(TARGET): $(OBJECTS)
	$(CC) -o $(TARGET) $(OBJECTS) $(LIBS)

%.o: %.c
	$(CC) $(CFLAGS) -c $<

clean:
	rm -f $(OBJECTS) $(TARGET)

.PHONY: clean
