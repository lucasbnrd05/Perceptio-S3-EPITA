#ifndef RESOLVE_H
#define RESOLVE_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_WORDS 100          // Maximum number of words to read
#define MAX_WORD_LENGTH 100    // Maximum length of a word

// Structure to store each word
typedef struct {
    char word[MAX_WORD_LENGTH];
} Word;

// Function declarations
int read_words(const char *filename, Word words[]);
int matrix_to_txt(char *filename, int **matrix, size_t h, size_t w);
void save_texture(const char* file_name, SDL_Renderer* renderer, SDL_Texture* texture);
int exec_resolve(int argc, char *argv[]);
int exec_resolve_with_grid_and_word(int argc, char *argv[]);

#endif // RESOLVE_H