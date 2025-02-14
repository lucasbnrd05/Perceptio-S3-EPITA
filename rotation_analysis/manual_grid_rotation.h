#ifndef MANUAL_GRID_ROTATION_H
#define MANUAL_GRID_ROTATION_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

// Function prototypes

/**
 * Loads an image from a file.
 * @param filename The name of the image file to load.
 * @return A pointer to the loaded SDL_Surface.
 */
SDL_Surface* loadImageRotation(const char* filename);

/**
 * Saves an SDL_Surface as a PNG image.
 * @param surface The SDL_Surface to save.
 * @param filename The name of the file to save the image to.
 */
void saveImageRotation(SDL_Surface* surface, const char* filename);

/**
 * Rotates an image by a specified angle.
 * @param src The source image to rotate.
 * @param angle_degrees The angle to rotate the image in degrees.
 * @return A pointer to the newly rotated SDL_Surface.
 */
SDL_Surface* rotateImage(SDL_Surface* src, double angle_degrees);

/**
 * Main function for executing the rotation.
 * @param argc The argument count.
 * @param argv The argument vector.
 * @return An integer indicating success (0) or failure (1).
 */
int exec_rotation(int argc, char* argv[]);

#endif // MANUAL_GRID_ROTATION_H
