#ifndef DETECTION_LETTER_H
#define DETECTION_LETTER_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <stdio.h>

// Function to check if a pixel is white enough
int is_white(Uint32 pixel_color, SDL_PixelFormat *format);

// Function to check if a vertical line in the surface is completely white
int isLineWhite(SDL_Surface *surface, int vline);

// Function to count the number of letters in the image based on non-
//white lines
int letter_counter(SDL_Surface *surface);

// Function to get the minimum and maximum y-coordinates of black pixels
// in a vertical line
void get_max_min_line(SDL_Surface *surface, int vline, int* min, int* max);

// Function to cut the detected letters into their bounding boxes
void letter_cutter(SDL_Surface *surface, int* letters_widths,
int* letters_base_x, int* letters_min_y, int* letters_max_y, size_t len);

// Main function to detect letters in an image and print their positions
int detect_letter(int argc, char *argv[], int coords[2][2],
int ***coorletter, int *taillemot);

// Function to free allocated memory
void free_detected_letters(int **coorletter, int count);

#endif // DETECTION_LETTER_H
