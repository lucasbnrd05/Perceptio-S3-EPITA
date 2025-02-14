#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <stdio.h>
#include <stdlib.h>

int x_start_line = 0;
int y_start_line = 0;
int x_end_line = 0;
int y_end_line = 0;
int taille_coorline = 0;

// Function to check if a pixel is sufficiently white
int iswhite(Uint32 pixel_color, SDL_PixelFormat *format) {
    Uint8 r = 0, g = 0, b = 0;
    SDL_GetRGB(pixel_color, format, &r, &g, &b);
    return (r > 230 && g > 230 && b > 230);
}

// Function to check if a line is white
int LineWhite(SDL_Surface *surface, int line) {
    Uint32 *pixels = (Uint32 *)surface->pixels;
    for (int x = x_start_line; x < x_end_line; x++)
    {
        Uint32 pixel = pixels[(line * surface->w) + x];
        if (!iswhite(pixel, surface->format))
        {
            return 0; // The line is not white
        }
    }
    return 1;
}

// Function to count the number of text lines
int line_counter(SDL_Surface *surface) {
    int counter = 0;
    int prevLineWhite = 1;

    for (int y = y_start_line; y < y_end_line; y++)
    {
        if (LineWhite(surface, y) == 0)
        {
            if (prevLineWhite == 1)
            {
                counter++;
            }
            prevLineWhite = 0;
        }
        else
        {
            prevLineWhite = 1;
        }
    }

    return counter;
}

// Function to obtain the start and end coordinates of text lines
void get_line_coords(SDL_Surface *surface, int **lineCoords) {
    int lineIndex = 0;
    int prevLineWhite = 1;

    for (int y = y_start_line; y < y_end_line; y++)
    {
        if (LineWhite(surface, y) == 0)
        {
            if (prevLineWhite == 1)
            {
                // Start of a new text line
                lineCoords[lineIndex] = malloc(4 * sizeof(int));
                lineCoords[lineIndex][0] = x_start_line;
                lineCoords[lineIndex][1] = y;
            }
            prevLineWhite = 0;
        }
        else
        {
            if (prevLineWhite == 0)
            {
                // End of a text line
                lineCoords[lineIndex][2] = x_end_line;
                lineCoords[lineIndex][3] = y - 1;
                lineIndex++;
            }
            prevLineWhite = 1; // line white
        }
    }

    // for the last text block
    if (prevLineWhite == 0)
    {
        lineCoords[lineIndex] = malloc(4 * sizeof(int));
        lineCoords[lineIndex][0] = x_start_line;
        lineCoords[lineIndex][1] = y_end_line;
        lineCoords[lineIndex][2] = x_end_line;
        lineCoords[lineIndex][3] = y_end_line - 1;
    }
}

// Main function to detect text lines
int detect_line(int argc, char *argv[], int coords[2][2],
 int ***lineCoords, int *lineCount) {
    if (argc != 2)
    {
        printf("Usage: %s <image.png>\n", argv[0]);
        return 1;
    }

    if (SDL_Init(SDL_INIT_VIDEO) != 0)
    {
        printf("SDL_Init Error: %s\n", SDL_GetError());
        return 1;
    }

    SDL_Surface *image = IMG_Load(argv[1]);
    if (!image)
    {
        printf("Error loading image: %s\n", IMG_GetError());
        SDL_Quit();
        return 1;
    }

    x_start_line = coords[0][0];
    y_start_line = coords[0][1];
    x_end_line = coords[1][0];
    y_end_line = coords[1][1];

    *lineCount = line_counter(image);
    //printf("Number of lines detected: %d\n", *lineCount);

    *lineCoords = malloc((*lineCount + 1) * sizeof(int *));
    if (*lineCoords == NULL)
    {
        fprintf(stderr, "Memory allocation error for line coordinates\n");
        SDL_FreeSurface(image);
        SDL_Quit();
        return 1;
    }

    get_line_coords(image, *lineCoords);



    SDL_FreeSurface(image);
    SDL_Quit();
    IMG_Quit();

    return 0;
}
