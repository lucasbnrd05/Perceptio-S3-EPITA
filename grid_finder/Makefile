# Variables de compilation
CC = gcc
CFLAGS = -Wall -Wextra
LIBS = -lSDL2 -lSDL2_image -fsanitize=address
TARGET = main

# Liste des fichiers sources et objets
SOURCES = main.c grid_finder.c detection_word.c detection_letter.c ../crop/crop_image.c
OBJECTS = $(SOURCES:.c=.o)

# Règle par défaut pour construire l'exécutable
$(TARGET): $(OBJECTS)
	$(CC) -o $(TARGET) $(OBJECTS) $(LIBS)

# Règle pour compiler les fichiers objets
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# Règle spécifique pour ../crop/crop_image.o (avec son chemin)
../crop/crop_image.o: ../crop/crop_image.c
	$(CC) $(CFLAGS) -c ../crop/crop_image.c -o ../crop/crop_image.o

# Nettoyage
clean:
	rm -f $(OBJECTS) $(TARGET)

# Éviter les conflits pour la règle 'clean'
.PHONY: clean
