#ifndef DETECTION_WORD_H
#define DETECTION_WORD_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <stdio.h>

// Function to check if a pixel is sufficiently white
int LineWhite(SDL_Surface *surface, int line);

// Function to check if a line is white
int iswhite(Uint32 pixel_color, SDL_PixelFormat *format);

// Function to count the number of text lines
int line_counter(SDL_Surface *surface);

// Function to obtain the start and end coordinates of text lines
void get_line_coords(SDL_Surface *surface, int **lineCoords);

// Main function to detect text lines
int detect_line(int argc, char *argv[], int coords[2][2],
int ***lineCoords, int *lineCount);


#endif // DETECTION_WORD_H
