#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <stdio.h>

// Function to save the surface as a PNG file
int SaveImage(SDL_Surface *surface, const char *filename) {
    if (IMG_SavePNG(surface, filename) != 0) {
        printf("Error saving image: %s\n", SDL_GetError());
        return -1;
    }
    printf("Image saved as %s\n", filename);
    return 0;
}

// Function to crop a part of an image and save it
void CropImage(const char *input_image, const char *output_image, \
SDL_Rect crop_rect) {
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        printf("SDL initialization error: %s\n", SDL_GetError());
        return;
    }

    if (!(IMG_Init(IMG_INIT_PNG) & IMG_INIT_PNG)) {
        printf("Error initializing SDL_image: %s\n", SDL_GetError());
        SDL_Quit();
        return;
    }

    SDL_Surface *image = IMG_Load(input_image);
    if (!image) {
        printf("Error loading image: %s\n", IMG_GetError());
        IMG_Quit();
        SDL_Quit();
        return;
    }

    // Check  crop dimensions
    if (crop_rect.x < 0 || crop_rect.y < 0 || \
    crop_rect.x + crop_rect.w > image->w || \
    crop_rect.y + crop_rect.h > image->h)
    {
        printf("Invalid cropping dimensions.\n");
        SDL_FreeSurface(image);
        IMG_Quit();
        SDL_Quit();
        return;
    }

    //  new surface -> cropped image
    SDL_Surface *cropped = SDL_CreateRGBSurface(0,
    crop_rect.w, crop_rect.h, image->format->BitsPerPixel,
    image->format->Rmask, image->format->Gmask,
    image->format->Bmask, image->format->Amask);

    if (!cropped) {
        printf("Error creating cropped surface: %s\n", SDL_GetError());
        SDL_FreeSurface(image);
        IMG_Quit();
        SDL_Quit();
        return;
    }

    // copy  cropped portion in image
    SDL_BlitSurface(image, &crop_rect, cropped, NULL);

    // image
    SaveImage(cropped, output_image);

    // free memory
    SDL_FreeSurface(cropped);
    SDL_FreeSurface(image);
    IMG_Quit();
    SDL_Quit();
}

int exec_crop(int argc, char *argv[]) {
    if (argc != 7) {
        printf("Usage: %s <input_image> <output_image> <x1> <y1> <x2> <y2>\n",\
        argv[0]);
        return 1;
    }

    const char *input_image = argv[1];
    const char *output_image = argv[2];

    // coordinates
    int x1 = atoi(argv[3]);
    int y1 = atoi(argv[4]);
    int x2 = atoi(argv[5]);
    int y2 = atoi(argv[6]);

    if (x1 >= x2 || y1 >= y2) {
        printf("Invalid crop coordinates. \
        (x1, y1) must be the top-left corner of (x2, y2).\n");
        return 1;
    }

    // Define crop rectangle
    SDL_Rect crop_rect;
    crop_rect.x = x1;
    crop_rect.y = y1;
    crop_rect.w = x2 - x1;
    crop_rect.h = y2 - y1;

    // Crop, save image
    CropImage(input_image, output_image, crop_rect);

    return 0;
}
