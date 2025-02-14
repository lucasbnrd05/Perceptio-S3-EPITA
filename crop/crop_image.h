#ifndef CROP_IMAGE_H
#define CROP_IMAGE_H

#include <SDL2/SDL.h>

// Save an SDL_Surface as a PNG file
int SaveImage(SDL_Surface *surface, const char *filename);

// Crop a specified rectangular area from an image and save it as a new image
void CropImage(const char *input_image, const char *output_image,\
 SDL_Rect crop_rect);

// Execute crop operation based on command-line arguments
int exec_crop(int argc, char *argv[]);

#endif // CROP_IMAGE_H
