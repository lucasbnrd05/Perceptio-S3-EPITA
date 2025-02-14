#include <SDL2/SDL_image.h>
#include <stdio.h>
#include <dirent.h>
#include <SDL2/SDL.h>
#include <ctype.h>

// Function to resize an image
SDL_Surface* resize_image(SDL_Surface* src, int new_width, int new_height) {
    // new surface with desired dimensions
    SDL_Surface* dst = SDL_CreateRGBSurface(0, new_width, new_height,
                                          src->format->BitsPerPixel,
                                          src->format->Rmask,
                                          src->format->Gmask,
                                          src->format->Bmask,
                                          src->format->Amask);

    if (dst == NULL) {
        fprintf(stderr, "Error creating destination surface: %s\n",
        SDL_GetError());
        return NULL;
    }

    // Resize image
    SDL_Rect src_rect = {0, 0, src->w, src->h};
    SDL_Rect dst_rect = {0, 0, new_width, new_height};
    SDL_BlitScaled(src, &src_rect, dst, &dst_rect);

    return dst;
}

int main(int argc, char* argv[]) {
    if(argc != 2) {
        printf("You must provide a directory as a parameter\n");
        return 1;
    }

    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        printf("Error initializing SDL: %s\n", SDL_GetError());
        return 1;
    }

    SDL_Surface* resized_image;
    SDL_Surface* image;

    struct dirent *dir;
    DIR *d = opendir(argv[1]);
    int size;
    if (d) {
        int i = 0;
        while ((dir = readdir(d)) != NULL) {
            if (dir->d_name[0] == '.') //ignored
                continue;

            char filepath[512];
            snprintf(filepath, sizeof(filepath), "%s%s", argv[1],
            dir->d_name);

            image = IMG_Load(filepath);
            if (image == NULL) {
                // Error handling
                printf("Error loading image: %s\n", filepath);
                continue;
            }

            // Resize image
            resized_image = resize_image(image, 28, 28);
            if (resized_image == NULL) {
                // Error handling
                printf("Error resizing image: %s\n", filepath);
                SDL_FreeSurface(image);
                continue;
            }

            // Save
            if (IMG_SavePNG(resized_image, filepath) != 0) {
                printf("Error saving image: %s\n", IMG_GetError());
            }

            i++;
        }
        closedir(d);
    }

    // Free
    SDL_FreeSurface(resized_image);
    SDL_FreeSurface(image);

    SDL_Quit();

    return 0;
}
