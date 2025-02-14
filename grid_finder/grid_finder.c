#include <err.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <stdio.h>
#include <stdlib.h>

#include "grid_finder.h"

// gcc -o grid_finder.o grid_finder.c -Wall -Wextra -lSDL2 -lSDL2_image

SDL_Surface *Surface = NULL;

/// This function frees all data to free
/// @param status this is the status the program exits with
void ExitProgram(const int status)
{
    if (!Surface)
        SDL_FreeSurface(Surface);
    SDL_Quit();
    IMG_Quit();
    exit(status);
}

/// Loads the surface from @param imagePath
/// @param imagePath this is the path to the image
void LoadSurface(const char *imagePath)
{
    if (!Surface)
        Surface = IMG_Load(imagePath);
}


/// Returns a boolean if the line is judged to be white
/// @param pixels array of pixels
/// @param format the format of the pixels
/// @param len the length of the line
/// @param step the space between pixels
/// @return 1 if the line is 98% white, else 0
char IsLineBlack(const Uint32 *pixels, const size_t len, const size_t step)
{
    size_t whitePixels = 0;
    for (size_t i = 0; i < len; i += step)
    {
        unsigned char r, g, b;
        const Uint32 pixel = pixels[i];
        SDL_GetRGB(pixel, Surface->format, &r, &g, &b);
        if (r == 0xff && g == 0xff && b == 0xff)
            whitePixels++;
    }

    if (whitePixels * 100 / 98 >= len / step)
        return 0;
    else
        return 1;
}

/// This function removes 1s surrounded by 0s
/// @param len the length of @param line
/// @param line an array of booleans
void RemoveLoneBlack(const size_t len, char *line)
{
    if (len < 2)
        return;
    if (!line[1])
        line[0] = 0;
    if (!line[len - 2])
        line[len - 1] = 0;
    for (size_t i = 1; i < len - 1; i++)
    {
        if (!line[i + 1] && !line[i - 1])
            line[i] = 0;
    }
}

/// This flattens vertically the Surface into @param line
/// @param line the resulted flat image
void FlattenImageV(char *line)
{
    const Uint32 *pixels = Surface->pixels;
    for (size_t i = 0; (int)i < Surface->w; i++)
    {
        line[i] = IsLineBlack(pixels + i, Surface->w * Surface->h,
        Surface->w);
    }
    RemoveLoneBlack(Surface->w, line);
}

/// This flattens horizontally the Surface into @param line
/// @param line the resulted flat image
void FlattenImageH(char *line)
{
    const Uint32 *pixels = Surface->pixels;
    for (size_t i = 0; (int)i < Surface->h; i++)
    {
        line[i] = IsLineBlack(pixels + i * Surface->w, Surface->w, 1);
    }
    RemoveLoneBlack(Surface->h, line);
}

/// @param line array of booleans
/// @param len size of @param line
/// @return the first occurrence of a 1
size_t FirstBlack(const char *line, const size_t len)
{
    size_t i;
    for (i = 0; i < len - 1 && !line[i]; i++) {}
    return i;
}

/// @param line array of booleans
/// @param len size of @param line
/// @return the last occurrence of a 1
size_t LastBlack(const char *line, const size_t len)
{
    size_t i;
    for (i = len - 1; i > 0 && !line[i]; i--) {}
    return i;
}

/// Calculates the average and mac gaps
/// a gap is consecutive zeros
/// @param line array of booleans
/// @param len length of @param line
/// @param max the beginning and end of the biggest gap
/// @return the average length of the gaps
size_t AverageGap(const char *line, const size_t len, size_t max[2])
{
    size_t average = 0;
    size_t count = 0;
    max[0] = max[1] = 0;
    for (size_t i = 0; i < len; i++)
    {
        size_t current;
        for (current = 0; i < len && !line[i]; i++, current++) {}
        if (current)
        {
            average += current;
            count++;
            if (current > max[1] - max[0] + 1)
            {
                max[0] = i - current;
                max[1] = i - 1;
            }
        }
    }
    if (count)
        average /= count;
    return average;
}

/// This flattens the Surface vertically, then finds first and last ones,
/// then finds the biggest gap, and calculates @param diff
/// @param diff this is the difference between biggest and average gaps
///     vertically in Surface
/// @param points array which contains :
///     points[0] = first one
///     points[1] = beginning of biggest gap
///     points[2] = end of biggest gap
///     points[3] = last one
/// @return an exit code
int FindGapV(size_t *diff, size_t points[4])
{
    char flatV[Surface->w];
    FlattenImageV(flatV);
    points[0] = FirstBlack(flatV, Surface->w);
    points[3] = LastBlack(flatV, Surface->w);
    if (points[0] <= points[3])
    {
        size_t average = AverageGap(flatV + points[0],
            points[3] - points[0] + 1, points+1);
        *diff = (points[2] - points[1]) - average;
        points[1] += points[0];
        points[2] += points[0];
        return EXIT_SUCCESS;
    }
    else
        return EXIT_FAILURE;
}

/// This flattens the Surface horizontally, then finds first and last ones,
/// then finds the biggest gap, and calculates @param diff
/// @param diff this is the difference between biggest and average gaps
///     horizontally in Surface
/// @param points array which contains :
///     points[0] = first one
///     points[1] = beginning of biggest gap
///     points[2] = end of biggest gap
///     points[3] = last one
/// @return an exit code
int FindGapH(size_t *diff, size_t points[4])
{
    char flatH[Surface->h];
    FlattenImageH(flatH);
    points[0] = FirstBlack(flatH, Surface->h);
    points[3] = LastBlack(flatH, Surface->h);
    if (points[0] <= points[3])
    {
        size_t average = AverageGap(flatH + points[0],
            points[3] - points[0] + 1, points+1);
        *diff = (points[2] - points[1]) - average;
        points[1] += points[0];
        points[2] += points[0];
        return EXIT_SUCCESS;
    }
    else
        return EXIT_FAILURE;
}

/// This computes the top left and bottom right corners
/// of the grid and word list
/// @param imagePath the path to the image
/// @param result an array to put the coordinates of the 4 points
/// @return 0 if success, 1 if failure
int exec_FindGrid(const char* imagePath, size_t result[8])
{
    if (SDL_Init(SDL_INIT_VIDEO))
        errx(EXIT_FAILURE, "Finder: Failed to init SDL: %s", SDL_GetError());
    if (!IMG_Init(IMG_INIT_PNG))
    {
        SDL_Quit();
        errx(EXIT_FAILURE, "Finder: Failed to init SDL_image: %s",
        IMG_GetError());
    }
    LoadSurface(imagePath);
    if (!Surface)
    {
        fprintf(stderr,"Finder: Failed to load image: %s", imagePath);
        ExitProgram(EXIT_FAILURE);
    }

    size_t pointsV[4];
    size_t diffV;
    if (FindGapV(&diffV, pointsV))
    {
        fprintf(stderr,"Finder: Picture looks nearly entirely white: %s",
        imagePath);
        ExitProgram(EXIT_FAILURE);
    }

    size_t pointsH[4];
    size_t diffH;
    if (FindGapH(&diffH, pointsH))
    {
        fprintf(stderr,"Finder: Picture looks nearly white: %s", imagePath);
        ExitProgram(EXIT_FAILURE);
    }


    if (diffV > diffH)
    {
          result[1] = 0;
        result[3] = Surface->h - 1;
        result[5] = 0;
        result[7] = Surface->h - 1;
        if (pointsV[1] + pointsV[2] > (size_t)(Surface->w))
        {
            result[0] = 0;
            result[2] = (pointsV[1] + pointsV[2]) / 2;
            result[4] = (pointsV[1] + pointsV[2]) / 2 + 1;
            result[6] = Surface->w - 1;
        }
        else
        {
            result[0] = (pointsV[1] + pointsV[2]) / 2 + 1;
            result[2] = Surface->w - 1;
            result[4] = 0;
            result[6] = (pointsV[1] + pointsV[2]) / 2;
        }
    }
    else
    {
        result[0] = 0;
        result[2] = Surface->w - 1;
        result[4] = 0;
        result[6] = Surface->w - 1;
        if (pointsH[1] + pointsH[2] > (size_t)(Surface->h))
        {
            result[1] = 0;
            result[3] = (pointsH[1] + pointsH[2]) / 2;
            result[5] = (pointsH[1] + pointsH[2]) / 2 + 1;
            result[7] = Surface->h - 1;
        }
        else
        {
            result[1] = (pointsH[1] + pointsH[2]) / 2 + 1;
            result[3] = Surface->h - 1;
            result[5] = 0;
            result[7] = (pointsH[1] + pointsH[2]) / 2;
        }
    }

    SDL_FreeSurface(Surface);
    SDL_Quit();
    IMG_Quit();
    return EXIT_SUCCESS;
}