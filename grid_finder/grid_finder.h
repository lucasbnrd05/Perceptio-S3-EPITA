#ifndef GRID_FINDER_H
#define GRID_FINDER_H

#include <err.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <stdio.h>
#include <stdlib.h>

/// This computes the top left and bottom right corners
/// of the grid and word list
/// @param imagePath the path to the image
/// @param result an array to put the coordinates of the 4 points
/// @return 0 if success, 1 if failure
int exec_FindGrid(const char* imagePath, size_t result[8]);

#endif // GRID_FINDER_H
