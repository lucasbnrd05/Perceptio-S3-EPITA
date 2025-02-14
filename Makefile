CC = gcc
CFLAGS = -Wall -Wextra -Werror `pkg-config --cflags gtk+-3.0` -g
LIBS = `pkg-config --libs gtk+-3.0` -lSDL2 -lSDL2_image -lm -lpthread -fsanitize=address
TARGET = interface_main

SOURCES = interface_main.c \
           solver_src/solver.c \
           image_binarization/binarization.c \
           nxor/ia_proof.c \
           neuron/neuron.c \
           rotation_analysis/manual_grid_rotation.c \
           crop/crop_image.c\
           grid_finder/grid_finder.c\
           grid_finder/detection_word.c\
           grid_finder/detection_letter.c\
           grid_finder/main.c\
           letter_ia/ia_letter.c\
           letter_ia/creation_dataset.c\
           resolve.c


SOLVER = solver_src/solver.c solver_src/main.c

OBJECTS = $(SOURCES:.c=.o)

all: $(TARGET) solver

$(TARGET): $(OBJECTS)
	$(CC) -o $(TARGET) $(OBJECTS) $(LIBS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

solver.o:
	$(CC) $(CFLAGS) -c solver.c -o $@

solver:
	$(CC) -o $@ $(SOLVER)

clean:
	rm -f $(OBJECTS) $(TARGET)
	rm -f solver

.PHONY: clean
