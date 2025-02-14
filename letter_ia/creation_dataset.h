#ifndef CREATION_DATASET_H
#define CREATION_DATASET_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <err.h>
#include "../image_binarization/binarization.h"

#define IMAGE_WIDTH 32 
#define IMAGE_HEIGHT 32 

typedef struct {
    SDL_Surface *image;
    int label;
    char name[256];
} LabeledImage;

// Prototypes of function
void extractNameAndLabel(const char *filename, char *name, int *label);
void loadImageWithLabel(const char *imagePath, int label, LabeledImage *dataset);
void saveDataset(LabeledImage *dataset, const char *filename);
int exec_creation(int argc, char *argv[]);


#endif // CREATION_DATASET_H