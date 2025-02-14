#ifndef BINARIZATION_H
#define BINARIZATION_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "../rotation_analysis/manual_grid_rotation.h"

// Function to convert an image to grayscale
void convertToGrayscale(SDL_Surface *surface);

// Function to calculate the local mean in a defined window around a pixel
double localMean(SDL_Surface *surface, unsigned long *integralImg,
int x, int y, int windowSize);

// Function to calculate the local standard deviation in a
//defined window around a pixel
double localStd(SDL_Surface *surface, unsigned long *integralImgSq,
 int x, int y, double mean, int windowSize);

// Function to initialize the integral image
void toIntegralImage(SDL_Surface *surface, unsigned long *integralImg);

// Function to initialize the squared integral image
void toIntegralImageSq(SDL_Surface *surface, unsigned long *integralImgSq);

// Function to increase the contrast of a grayscaled image
void increaseGrayContrast(SDL_Surface *surface);

// Function for Sauvola thresholding on a grayscaled image
void sauvolaThresholding(SDL_Surface *surface);

// Function to apply Gaussian blur to the image
void apply_gaussian_blur(SDL_Surface *surface);

// Function that transform a pixel in white depending on the number of
// white neighbour
void simpleThinning(SDL_Surface *surface, int threshold, int newColor);

// Function that counts the number of black ajacent pixels to a pixel
// with the coods (x, y)
int countBlack(SDL_Surface *surface, int x, int y);

// Function that transforms a 'visited' pixel to the 'newGray' color
void visitedToGray(SDL_Surface *surface, int x, int y, Uint8 newGray);

// Function that changes a pixel to white if it is directly connected
//to to many or not enough black pixels
void simpleDenoising(SDL_Surface *surface);

// Function to calculate the average brightness of the image
Uint8 calculateAverageBrightness(SDL_Surface *surface);

// Function to calculate the local threshold
Uint8 calculateLocalThreshold(SDL_Surface *surface, int x, int y);

// Function to convert the image to black and white using a dynamic threshold
void convertToBlackAndWhite(SDL_Surface *surface);

// Function to apply a median filter to the image for noise reduction
void apply_median_filter(SDL_Surface *surface);

// Function to verify if a pixel of a preprocessed image is a "edge pixel"
int isEdgePixel(SDL_Surface *surface, int x, int y);

// Function to detect the angle of a rotated preproced image
int angleFinder(SDL_Surface *surface);

// Function to save the processed image with a new filename
void saveImage(SDL_Surface *surface, const char *originalFilename);

// Main function to handle image processing, should be called with arguments
int imagePreProcessing(int argc, char *argv[]);

#endif // BINARIZATION_H
