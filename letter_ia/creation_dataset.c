#include <dirent.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <ctype.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <err.h>
#include "../image_binarization/binarization.h"


/*
HOW TO COMPILE:
    Go to the level -1, so the directory above
    Run the following command:  gcc letter_ia/creation_dataset.c -o letter_ia/creation_dataset  image_binarization/binarization.c -lm -lSDL2 -lSDL2_image -Wall -Werror
*/

#define IMAGE_WIDTH 32 
#define IMAGE_HEIGHT 32 

typedef struct {
    SDL_Surface *image;
    int label;
    char name[256];
} LabeledImage;

LabeledImage dataset[10000]; 
int num_images = 0;

//extract name and label
void extractNameAndLabel(const char *filename, char *name, int *label) 
{
    int len = strlen(filename);
    int underscorePos = -1;

    for (int i = 0; i < len; i++) 
    {
        if (filename[i] == '_') 
        {
            underscorePos = i;
            break;
        }
    }

    if (underscorePos == -1 || underscorePos + 1 >= len) 
    {
        fprintf(stderr, "Error: filename format is incorrect: %s\n", filename);
        strcpy(name, filename);
        *label = -1;
        return;
    }

    strncpy(name, filename, underscorePos);
    name[underscorePos] = '\0';

    *label = atoi(&filename[underscorePos + 1]);
    if(*label == 0)
        printf("%s\n",filename);
}

// function to load image
void loadImageWithLabel(const char *imagePath, int label, LabeledImage *dataset) 
{
    SDL_Surface *image = IMG_Load(imagePath);
    if (!image) 
    {
        fprintf(stderr, "Error loading image: %s\n", IMG_GetError());
        return;
    }

    dataset[num_images].image = image;
    dataset[num_images].label = label;
    strncpy(dataset[num_images].name, imagePath, sizeof(dataset[num_images].name) - 1);
    dataset[num_images].name[sizeof(dataset[num_images].name) - 1] = '\0';
    num_images++;
}

// save file
void saveDataset(LabeledImage *dataset, const char *filename) 
{

    // read number of image
    FILE *file = fopen(filename, "r");

    int existing_count = 0;
    if (file) {
        fscanf(file, "%d\n", &existing_count); // Lire le nombre d'images
        fclose(file);
    } else {
        // if file not exist
        existing_count = 0;
    }

    file = fopen(filename, "a"); 

    if (!file) {
        fprintf(stderr, "Error opening save file\n");
        return;
    }

    for (int i = 0; i < num_images; i++) {
        if (dataset[i].label != -1) {

            int width = dataset[i].image->w;
            int height = dataset[i].image->h;
            char tab[1024];            
            if (width > 32 || height > 32) 
            {
                errx(1, "Error: Image too large. %s", dataset[i].name);
            }
            // Initialize the output array to zero
            for (int i = 0; i < 1024; i++) {
                tab[i] = '0';
            }

            /*convertToGrayscale(dataset[i].image);
            increaseGrayContrast(dataset[i].image);
            apply_gaussian_blur(dataset[i].image);
            sauvolaThresholding(dataset[i].image);
            for(int j = 0; j<3 ; j++){
                simpleThinning(dataset[i].image, 6, 255);
            }
            simpleDenoising(dataset[i].image);*/
            // Fill the array based on the image pixels
            for (int y = 0; y < height; y++) 
            {
                for (int x = 0; x < width; x++) 
                {
                   Uint32 pixel = ((Uint32 *)dataset[i].image->pixels)[y * width + x];
                    Uint8 r, g, b;
                    SDL_GetRGB(pixel, dataset[i].image->format, &r, &g, &b);
                    Uint8 brightness = (Uint8)(0.299 * r + 0.587 * g + 0.114 * b);
                    tab[y * 32 + x] = (brightness < 128) ? '1' : '0';  //  value to the  array
                }
            }
            for (int y = 0; y < 1024; y++) 
            {
                    fprintf(file, "%c", tab[y]);
            }
            fprintf(file, ";%d;\n", dataset[i].label); // add the label
        }
    }


    fclose(file);

    // modiffy first line with number of image
    existing_count += num_images; // add actual count
    file = fopen(filename, "r+"); 
    if (file) {
        fprintf(file, "%d\n", existing_count); 
        fclose(file);
    }
}



int exec_creation(int argc, char *argv[]) 
{
    if (argc < 2) 
    {
        fprintf(stderr, "The format to follow is ./executable_name <directory>\n");
        return EXIT_FAILURE;
    }

    if (SDL_Init(SDL_INIT_VIDEO) != 0) 
    {
        fprintf(stderr, "Error initializing SDL: %s\n", SDL_GetError());
        return EXIT_FAILURE;
    }

    int flags = IMG_INIT_PNG;
    if (!(IMG_Init(flags) & flags)) 
    {
        fprintf(stderr, "Error initializing SDL_image: %s\n", IMG_GetError());
        SDL_Quit();
        return EXIT_FAILURE;
    }
  
    
    struct dirent *dir;
    DIR *d = opendir(argv[1]);
    if (d) 
    {
        while ((dir = readdir(d)) != NULL) 
        {
            if (dir->d_name [0] != '.')             
            { 
                char imagePath[512];
                snprintf(imagePath, sizeof(imagePath), "%s/%s", argv[1], dir->d_name);
                char name[512];
                int label;
                extractNameAndLabel(dir->d_name, name, &label);
                loadImageWithLabel(imagePath, label, dataset);
            }
        }
        closedir(d);
    } 
    else 
    {
        fprintf(stderr, "Error opening directory: %s\n", argv[1]);
        SDL_Quit();
        return EXIT_FAILURE;
    }

    saveDataset(dataset, "source_file/data_base/dataset_labels.txt");
       

    // Free
    for (int i = 0; i < num_images; i++) 
    {
        SDL_FreeSurface(dataset[i].image);
    }

    IMG_Quit();
    SDL_Quit();
    return EXIT_SUCCESS;
}