#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

#define PI 3.14159265358979323846  // PI constant

// Function to load an image
SDL_Surface* loadImageRotation(const char* filename) {
    SDL_Surface* image = IMG_Load(filename);
    if (!image) {
        fprintf(stderr, "Error loading image %s: %s\n",
        filename, IMG_GetError());
        exit(1);
    }
    return image;
}

// Function to save an image as PNG
void saveImageRotation(SDL_Surface* surface, const char* filename) {
    if (IMG_SavePNG(surface, filename) != 0) {
        fprintf(stderr, "Error saving image: %s\n", IMG_GetError());
        exit(1);
    }
}

// Function to rotate the image
SDL_Surface* rotateImage(SDL_Surface* src, double angle_degrees) {
    double angle = angle_degrees * PI / 180.0;  //angle to radians
    double cos_angle = cos(angle);  //cosine of the angle
    double sin_angle = sin(angle);  //sine of the angle

    // Calculate  dimensions -> image after rotation
    int new_width = (int)(fabs(src->w * cos_angle) + \
    fabs(src->h * sin_angle));
    int new_height = (int)(fabs(src->w * sin_angle) + \
    fabs(src->h * cos_angle));

    // Allocate a new surface -> rotated image
    SDL_Surface* rotated = SDL_CreateRGBSurface(0, new_width,
    new_height, 32, 0x00FF0000, 0x0000FF00, 0x000000FF, 0xFF000000);
    if (!rotated) {
        fprintf(stderr, "Error creating surface: %s\n", SDL_GetError());
        exit(1);
    }

    // Coordinates of the center of the original image and the resulting image
    int cx = src->w / 2;
    int cy = src->h / 2;
    int new_cx = new_width / 2;
    int new_cy = new_height / 2;

    // Fill the rotated image with pixels from the original image
    Uint32* src_pixels = (Uint32*)src->pixels;
    Uint32* rotated_pixels = (Uint32*)rotated->pixels;

    for (int y = 0; y < new_height; y++) {
        for (int x = 0; x < new_width; x++) {
            int xt = x - new_cx;  //coordinates in the rotated image
            int yt = y - new_cy;

            //corresponding coordinates -> original image
            int src_x = (int)(cos_angle * xt - sin_angle * yt) + cx;
            int src_y = (int)(sin_angle * xt + cos_angle * yt) + cy;

            // Copy pixel if coordinates valid
            if (src_x >= 0 && src_x < src->w && src_y >= 0 && src_y < src->h) {
                rotated_pixels[y * new_width + x] = \
                src_pixels[src_y * src->w + src_x];
            }


            else {
                rotated_pixels[y * new_width + x] = \
                SDL_MapRGB(rotated->format, 255, 255, 255);  // Fill with white
            }
        }
    }

    return rotated;
}

int exec_rotation(int argc, char* argv[]) {
    if (argc != 4) {
        fprintf(stderr,
        "Usage: %s <input.jpg/png> <output.png> <angle_degrees>\n", argv[0]);
        return 1;
    }

    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        fprintf(stderr, "Error initializing SDL: %s\n", SDL_GetError());
        return 1;
    }
    if (!IMG_Init(IMG_INIT_JPG | IMG_INIT_PNG)) {
        fprintf(stderr, "Error initializing SDL_image: %s\n", IMG_GetError());
        return 1;
    }

    SDL_Surface* img = loadImageRotation(argv[1]);

    // Rotate image angle
    double angle = atof(argv[3]);
    SDL_Surface* rotated = rotateImage(img, angle);

    // Save
    saveImageRotation(rotated, argv[2]);

    // Free
    SDL_FreeSurface(img);
    SDL_FreeSurface(rotated);

    IMG_Quit();
    SDL_Quit();

    return 0;
}
