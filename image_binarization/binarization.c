#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <math.h>
#include "binarization.h"

//#define WINDOW_SIZE 5
#define PI 3.14159265358979323846  // PI constant


// Function to convert an image to grayscale
void convertToGrayscale(SDL_Surface *surface) {
    Uint32 *pixels = (Uint32 *)surface->pixels;
    for (int y = 0; y < surface->h; y++) {
        for (int x = 0; x < surface->w; x++) {
            Uint32 pixel = pixels[(y * surface->w) + x];
            Uint8 r, g, b;
            SDL_GetRGB(pixel, surface->format, &r, &g, &b);

        // Calculate the gray value using a weighted average (the Luma one)
            Uint8 gray = (Uint8)(0.2126 * r + 0.7152 * g + 0.0722 * b);
            Uint32 grayPixel = SDL_MapRGB(surface->format, gray, gray, gray);
               pixels[(y * surface->w) + x] = grayPixel;
        }
    }
}

// Calculate the average grayscale value (local mean)
double localMean(SDL_Surface *surface, unsigned long *integralImg,
int x, int y, int windowSize) {
    int w = surface->w;
    int h = surface->h;
    //25
    //int windowSize = 25;
    int halfWindow = windowSize / 2;

    int x1 = (x + halfWindow >= w)? w - 1 : x + halfWindow;
    int x2 = (x - halfWindow <= 0)? 0 : x - halfWindow;
    int y1 = (y + halfWindow >= h)? h - 1 : y + halfWindow;
    int y2 = (y - halfWindow <= 0)? 0 : y - halfWindow;

    unsigned long a =                           integralImg[x1 + (y1 * w)];
    unsigned long b = (y2 <= 0 || x2 <= 0)? 0 : integralImg[x2 + (y2 * w)];
    unsigned long c = (y2 <= 0)? 0 :            integralImg[x1 + (y2 * w)];
    unsigned long d = (x2 <= 0)? 0 :            integralImg[x2 + (y1 * w)];
    int count = (x1 - x2 + 1) * (y1 - y2 + 1);

    // Calculate the mean using intergral image
    double localMean = (a + b - c - d) / count;

    return localMean;
}

double localStd(SDL_Surface *surface, unsigned long *integralImgSq,
int x, int y, double mean, int windowSize){
    int w = surface->w;
    int h = surface->h;
    // 15
    //int windowSize = 25;
    int halfWindow = windowSize / 2;

    int x1 = (x + halfWindow >= w)? w - 1 : x + halfWindow;
    int x2 = (x - halfWindow <= 0)? 0 : x - halfWindow;
    int y1 = (y + halfWindow >= h)? h - 1 : y + halfWindow;
    int y2 = (y - halfWindow <= 0)? 0 : y - halfWindow;

    unsigned long a =                           integralImgSq[x1 + (y1 * w)];
    unsigned long b = (y2 <= 0 || x2 <= 0)? 0 : integralImgSq[x2 + (y2 * w)];
    unsigned long c = (y2 <= 0)? 0 :            integralImgSq[x1 + (y2 * w)];
    unsigned long d = (x2 <= 0)? 0 :            integralImgSq[x2 + (y1 * w)];
    int count = (x1 - x2 + 1) * (y1 - y2 + 1);

    double localvariance = ((a + b - c - d) / count) - (mean * mean);

//printf("Pixel (%d, %d): a = %ld, b = %ld, c = %ld, d = %ld, localvariance = %.2f\n",
//       x, y, a, b, c, d, localvariance);

    return sqrtf(localvariance);
}


void toIntegralImg(SDL_Surface *surface, unsigned long *integralImg) {
    Uint32 *pixels = (Uint32 *)surface->pixels;
    int w = surface->w;
    int h = surface->h;
    // ok donc le pb la c'est que je cast pas en Uint 8 pout chaque pixel
    for (int y = 0; y < h; y++) {
        for (int x = 0; x < w; x++) {
            unsigned long upperPixel = (y > 0) ? \
                integralImg[((y - 1) * w) + x] : 0;
            unsigned long leftPixel = (x > 0) ? \
                integralImg[(y * w) + (x - 1)] : 0;
            unsigned long diagonalPixel = (x > 0 && y > 0) ?\
                integralImg[((y - 1) * w) + (x - 1)] : 0;
            unsigned long sum = (Uint8) pixels[(y * w) + x] +
                         leftPixel +
                         upperPixel -
                         diagonalPixel;
            integralImg[(y * w) + x] = sum;
        }
    }
}

void toIntegralImgSq(SDL_Surface *surface, unsigned long *integralImgSq) {
    Uint32 *pixels = (Uint32 *)surface->pixels;
    int w = surface->w;
    int h = surface->h;

    for (int y = 0; y < h; y++) {
        for (int x = 0; x < w; x++) {
            unsigned long upperPixel = (y > 0) ?\
                integralImgSq[((y - 1) * w) + x] : 0;
            unsigned long leftPixel = (x > 0) ? \
                integralImgSq[(y * w) + (x - 1)] : 0;
            unsigned long diagonalPixel = (x > 0 && y > 0) ?\
                integralImgSq[((y - 1) * w) + (x - 1)] : 0;
            integralImgSq[(y * w) + x] = \
            ((Uint8)pixels[(y * w) + x] * (Uint8)pixels[(y * w) + x]) +
            leftPixel +
            upperPixel -
            diagonalPixel;
        }
    }
}

//Function to increase the contrast of an grayscaled image
void increaseGrayContrast(SDL_Surface *surface) {
    Uint32 *pixels = (Uint32 *)surface->pixels;
    int w = surface->w;
    int h = surface->h;
    //3.5
    double contrastFactor = 10.5;

    unsigned long *integralImg = malloc(w * h * sizeof(unsigned long));
    if (!integralImg) {
        printf("Memory allocation failed!\n");
        return;
    }
    toIntegralImg(surface, integralImg);

    double mean = localMean(surface, integralImg, w/2, h/2, fmax(w/2, h/2));
    for (int y = 0; y < surface->h; y++) {
        for (int x = 0; x < surface->w; x++) {

            Uint8 gray = (Uint8)pixels[(y * surface->w) + x];
            // Calculate the new contrast value and the local mean

            long newContrast = (gray - mean) * contrastFactor + mean;
            // Limit the values if they go to high or too low
            newContrast = (newContrast > 255) ? 255 : newContrast;
            newContrast = (newContrast < 0) ? 0 : newContrast;

            Uint32 contrastPixel = SDL_MapRGB(surface->format,
            (Uint8)newContrast,
            (Uint8)newContrast,
            (Uint8)newContrast);
            pixels[(y * surface->w) + x] = contrastPixel;
        }
    }
}

void sauvolaThresholding(SDL_Surface *surface) {
    Uint32 *pixels = (Uint32 *)surface->pixels;
    int width = surface->w;
    int height = surface->h;
    // Sensivity parameter, often between 0.2 and 0.5
    float k = 0.6;
    // Maximum value of the standard deviation, usually R = 128
    float R = 128;

    unsigned long *integralImg = malloc(width * height * \
        sizeof(unsigned long));
    if (!integralImg) {
        printf("Memory allocation failed!\n");
        return;
    }
    toIntegralImg(surface, integralImg);

    unsigned long *integralImgSq = malloc(width * height *
        sizeof(unsigned long));
    if (!integralImgSq) {
        printf("Memory allocation failed!\n");
        return;
    }
    toIntegralImgSq(surface, integralImgSq);

    for (int y = 1; y < height - 1; y++) {
        for (int x = 1; x < width - 1; x++) {

            Uint32 pixel = pixels[y * width + x];
            Uint8 r, g, b;
            SDL_GetRGB(pixel, surface->format, &r, &g, &b);

            // Calculate local mean and standard deviation
            float mean = localMean(surface, integralImg, x, y, 25);
            float std = localStd(surface, integralImgSq, x, y, mean, 25);

            // Binarization: if the gray value is above the dynamic
            //threshold, white; otherwise, black
            float dynamicThreshold = mean * (1 + k * ((std /R) - 1));

            Uint8 value = (r > dynamicThreshold) ? 255 : 0;
            Uint32 bwPixel = SDL_MapRGB(surface->format, value, value, value);
            pixels[y * width + x] = bwPixel;

        }
    }

    free(integralImg);
    free(integralImgSq);
}

// Function to apply gaussian blur
void apply_gaussian_blur(SDL_Surface *surface) {
    Uint32 *pixels = (Uint32 *)surface->pixels;
    int width = surface->w;
    int height = surface->h;

    Uint32 *copy = malloc(width * height * sizeof(Uint32));
    if (!copy) {
        printf("Memory allocation failed!\n");
        return;
    }
    // Copies the pixels into copy
    memcpy(copy, pixels, width * height * sizeof(Uint32));

    // Gaussian kernel matrix
    float kernel[3][3] = {
        {1/16.0, 1/8.0, 1/16.0},
        {1/8.0, 1/4.0, 1/8.0},
        {1/16.0, 1/8.0, 1/16.0}
    };

    for (int y = 1; y < height - 1; y++) {
        for (int x = 1; x < width - 1; x++) {
            float sum_r = 0, sum_g = 0, sum_b = 0, sum_a = 0;

            for (int j = -1; j <= 1; j++) {
                for(int i = -1; i <= 1; i++) {
                    Uint32 pixel = copy[(y + j) * width + (x + i)];
                    Uint8 r, g, b, a;
                    SDL_GetRGBA(pixel, surface->format, &r, &g, &b, &a);

                    float weight = kernel[j + 1][i + 1];
                    sum_r += r * weight;
                    sum_g += g * weight;
                    sum_b += b * weight;
                    sum_a += a * weight;
                }
            }

            pixels[y * width + x] = SDL_MapRGBA(surface->format,
                (Uint8)sum_r, (Uint8)sum_g, (Uint8)sum_b, (Uint8)sum_a);
        }
    }

    free(copy);
}

// Function that transform a pixel in white depending on the number
// of white neighbour
void simpleThinning(SDL_Surface *surface, int whiteThreshold, int newColor) {
    Uint32 *pixels = (Uint32 *)surface->pixels;
    Uint32 *copy = malloc(surface->w * surface->h * sizeof(Uint32));
    if (!copy) {
        printf("Memory allocation failed!\n");
        return;
    }
    // Copies the pixels into copy
    memcpy(copy, pixels, surface->w * surface->h * sizeof(Uint32));

    for (int y = 0; y < surface->h - 1; y++) {
        for (int x = 0; x < surface->w - 1; x++) {
            int count = 0;
                for (int j = -1; j <= 1; j++) {
                    for(int i = -1; i <= 1; i++) {
                        Uint8 value = (Uint8)copy[((j + y) * \
                            surface->w) + (i + x)];
                        if (value == newColor)
                            count++;
                    }
                }


            if (count >= whiteThreshold)
                pixels[(y * surface->w) + x] = SDL_MapRGB(surface->format,
                    newColor, newColor, newColor);
            //else
                //printf("pixel (%d,%d)white count : %d\n", x, y, whiteCount);
        }
    }
    free(copy);
}

// Function that counts the number of black ajacent pixels to a
//pixel with the coods (x, y)
int countBlack(SDL_Surface *surface, int x, int y) {
    Uint32 *pixels = (Uint32 *)surface->pixels;
    int w = surface->w;
    int h = surface->h;
    int count = 0;

    Uint8 value = (Uint8)pixels[(y * surface->w) + x];
    if (value == 0){
        count++;
        // Mark the pixel as 'visited' with the value 42
        pixels[(y * surface->w) + x] = 42;
        for (int j = -1; j <= 1; j++) {
            for (int i = -1; i <= 1; i++) {
                // Boudaries check
                if ((i + x) < w && (i + x) > 0 &&
                    (j + y) < h && (j + y) > 0)
                    count+= countBlack(surface, (i + x), (j + y));
            }
        }
    }
    return count;
}

// Function that transforms a 'visited' pixel to the 'newGray' color
void visitedToGray(SDL_Surface *surface, int x, int y, Uint8 newGray) {
    Uint32 *pixels = (Uint32 *)surface->pixels;
    int w = surface->w;
    int h = surface->h;
    Uint8 value = (Uint8)pixels[(y * surface->w) + x];

    // checks if the pixel is 'visited' with the value 42
    if (value == 42){
        // transform the 'visited pixel' to the new color
        pixels[(y * surface->w) + x] = SDL_MapRGB(surface->format,
            newGray, newGray, newGray);
        for (int j = -1; j <= 1; j++) {
            for (int i = -1; i <= 1; i++) {
                if ((i + x) < w && (i + x) > 0 &&
                    (j + y) < h && (j + y) > 0)
                    visitedToGray(surface, (i + x), (j + y), newGray);
            }
        }
    }
}


// Function that changes a pixel to white if it is directly connected
//to to many or not enough black pixels
void simpleDenoising(SDL_Surface *surface) {
    int w = surface->w;
    int h = surface->h;

    int minBlack = 25;
    int maxBlack = 2000;
    for (int y = 0; y < h; y++) {
        for (int x = 0; x < w; x++) {

            int count = countBlack(surface, x, y);
            if (count > minBlack && count < maxBlack)
                visitedToGray(surface, x, y, 0);
            else
                visitedToGray(surface, x, y, 255);
        }
    }
}

// Function to calculate the average brightness of the image
Uint8 calculateAverageBrightness(SDL_Surface *surface) {
    Uint32 *pixels = (Uint32 *)surface->pixels;
    Uint64 totalBrightness = 0;
    int numPixels = surface->w * surface->h;

    for (int y = 0; y < surface->h; y++) {
        for (int x = 0; x < surface->w; x++) {
            Uint32 pixel = pixels[(y * surface->w) + x];
            Uint8 r, g, b;
            SDL_GetRGB(pixel, surface->format, &r, &g, &b);
            Uint8 gray = (Uint8)(0.3 * r + 0.59 * g + 0.11 * b);
            totalBrightness += gray;
        }
    }

    return (Uint8)(totalBrightness / numPixels);
}



// Calculate the average grayscale value (local threshold)
Uint8 calculateLocalThreshold(SDL_Surface *surface, int x, int y) {
    Uint32 *pixels = (Uint32 *)surface->pixels;
    int w = surface->w;
    int h = surface->h;
    int windowSize = 200;
    int halfWindow = windowSize / 2;

    int sum = 0;
    int count = 0;

    // Loop through the 15x15 neighborhood
    for (int wy = -halfWindow; wy <= halfWindow; wy++) {
        for (int wx = -halfWindow; wx <= halfWindow; wx++) {
            int nx = x + wx;
            int ny = y + wy;

            // Ensure the neighbor coordinates are within the image bounds
            if (nx >= 0 && nx < w && ny >= 0 && ny < h) {
                Uint32 neighborPixel = pixels[ny * w + nx];
                Uint8 neighborGray;
                SDL_GetRGB(neighborPixel, surface->format, &neighborGray,
                &neighborGray, &neighborGray);

                // Accumulate grayscale values
                sum += neighborGray;
                count++;
            }
        }
    }

    // Calculate the average grayscale value (local threshold)
    Uint8 localThreshold = sum / count;
    return localThreshold;
}

// Function to convert the image to black and white (binarization)
//using a dynamic threshold
void convertToBlackAndWhite(SDL_Surface *surface) {
    Uint32 *pixels = (Uint32 *)surface->pixels;
    for (int y = 0; y < surface->h; y++) {
        for (int x = 0; x < surface->w; x++) {
            Uint32 pixel = pixels[(y * surface->w) + x];
            Uint8 r, g, b;
            SDL_GetRGB(pixel, surface->format, &r, &g, &b);

            // Binarization: if the gray value is above the dynamic
            //threshold, white; otherwise, black

            Uint8 dynamicThreshold = calculateLocalThreshold(surface, x, y);

            Uint8 value = (r > dynamicThreshold) ? 255 : 0;
            Uint32 bwPixel = SDL_MapRGB(surface->format, value, value, value);
            pixels[(y * surface->w) + x] = bwPixel;
        }
    }
}


// Returns a negative number if first < second
int pixelCmp(const void *a, const void *b ) {
    int first = *(const Uint32 *) a;
    int second = *(const Uint32 *) b;
    return first - second;
}


void apply_median_filter(SDL_Surface *surface) {
    Uint32 *pixels = (Uint32 *)surface->pixels;
    int width = surface->w;
    int height = surface->h;

    Uint32 *copy = malloc(width * height * sizeof(Uint32));
    if (!copy) {
        printf("Memory allocation failed!\n");
        return;
    }
    // Copies the pixels into copy
    memcpy(copy, pixels, width * height * sizeof(Uint32));

    // array of all the pixel values around
    Uint32 median_array[9];

    for (int y = 1; y < height - 1; y++) {
        for (int x = 1; x < width - 1; x++) {
            size_t k = 0;
            for (int j = -1; j <= 1; j++) {
                for(int i = -1; i <= 1; i++) {
                    Uint32 pixel = copy[(y + j) * width + (x + i)];
                    median_array[k] = pixel;
                    k++;
                }
            }
            qsort(median_array, 9, sizeof(Uint32), pixelCmp);
            pixels[y * width + x] = median_array[4];
        }
    }

    free(copy);
}


// Returns 1 if the pixel is a border, 0 otherwise
int isEdgePixel(SDL_Surface *surface, int x, int y)
{
    Uint32 *pixels = (Uint32 *)surface->pixels;
    int width = surface->w;
    Uint8 currPixel = (Uint8)pixels[(y * width) + x];
    int whiteCount = 0;

    // If the pixel is white it can't be an edge
    if (currPixel == (Uint8)(255))
        return 0;

    for (int j = -1; j <= 1; j++) {
        for(int i = -1; i <= 1; i++) {
            Uint8 value = (Uint8)pixels[((j + y) * width) + (i + x)];
            
            // if the black pixel has a white neighbour, returns true
            if (value == 255) {
                whiteCount++;
                if (whiteCount > 1) {
                    return 1;
                }
            }
        }
    }

    return 0;
}

// returns the angle in degrees of which the image has to be tilted
int angleFinder(SDL_Surface *surface) 
{
    //Uint32 *pixels = (Uint32 *)surface->pixels;
    int width = surface->w;
    int height = surface->h;
    int diagonal = (int)sqrt((width * width) + (height * height));
    int incr = 1;

    Uint32 *accumulator = calloc((180 / incr)*(diagonal * 2), sizeof(Uint32));
    if (!accumulator) {
        printf("Memory allocation failed!\n");
        return -1;
    }

    for (int y = 1; y < height - 1; y++) {
        for (int x = 1; x < width - 1; x++) {
            if (isEdgePixel(surface, x, y) == 1) {

                //pixels[(y * width) + x] = SDL_MapRGB(surface->format, 0, 255, 0);
                for (int theta = 0; theta < (180 / incr); theta ++) {

                    float thetaRad = theta * PI / 180.0;
                    int rho = (int)(x * cos(thetaRad) + y * sin(thetaRad));
                    
                    rho += diagonal;
                    accumulator[rho * (180 / incr) + 
                                theta] += 1;
                }
            }
        }
    }

    int peakAngle = 0;
    int maxVote = 0;
    for (int rho = 0; rho < (diagonal * 2); rho ++) {
        for (int theta = 0; theta < (180 / incr); theta += incr) {
            
            int currVote = accumulator[rho * (180 / incr) + 
                           theta];
            
            if(currVote > maxVote) {
                maxVote = currVote;
                peakAngle = theta;
            }
        }
    }

    free(accumulator);
    return peakAngle;
}




// Function to save the image under a new name
void saveImage(SDL_Surface *surface, const char *originalFilename) {
    char newFilename[256];
    snprintf(newFilename, sizeof(newFilename), "%s_grayscaled.png",
    originalFilename);

    if (IMG_SavePNG(surface, newFilename) != 0) {
        fprintf(stderr, "Error saving image: %s\n", IMG_GetError());
    } else {
        //printf("Image saved as: %s\n", newFilename);
    }
}

int imagePreProcessing(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <image_path>\n", argv[0]);
        return EXIT_FAILURE;
    }

    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        fprintf(stderr, "Error initializing SDL: %s\n", SDL_GetError());
        return EXIT_FAILURE;
    }

    int flags = IMG_INIT_PNG;
    if (!(IMG_Init(flags) & flags)) {
        fprintf(stderr, "Error initializing SDL_image: %s\n", IMG_GetError());
        SDL_Quit();
        return EXIT_FAILURE;
    }

    SDL_Surface *image = IMG_Load(argv[1]);
    if (!image) {
        fprintf(stderr, "Error loading image: %s\n", IMG_GetError());
        IMG_Quit();
        SDL_Quit();
        return EXIT_FAILURE;
    }

    //printf("Dynamic threshold calculated: %d\n", dynamicThreshold);

    for(int i = 0; i<1 ; i++) {

        convertToGrayscale(image);
        increaseGrayContrast(image);
        apply_gaussian_blur(image);
        sauvolaThresholding(image);
        for(int i = 0; i<3 ; i++){
            simpleThinning(image, 6, 255);
        }
        simpleDenoising(image);
    }

    // Finds the correct angle and rotates the original image
    double rotationAngle = angleFinder(image);
    if (rotationAngle != 180 && rotationAngle != 90 &&
        rotationAngle != 0 && rotationAngle != 270) {
        image = rotateImage(image, rotationAngle);
        printf("the needed angle is:%f\n", rotationAngle);
    }
    else
        printf("the needed angle is:%f\n", 0.0);
    

    //for(int i = 0; i<10 ; i++)
    //    denoise_image(image);

    saveImage(image, argv[1]);

    SDL_Window *window = SDL_CreateWindow("Black and White Image",
    SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, image->w, image->h, 0);
    if (!window) {
        fprintf(stderr, "Error creating window: %s\n", SDL_GetError());
        SDL_FreeSurface(image);
        IMG_Quit();
        SDL_Quit();
        return EXIT_FAILURE;
    }

    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1,
    SDL_RENDERER_SOFTWARE);
    SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, image);

    SDL_RenderClear(renderer);
    SDL_RenderCopy(renderer, texture, NULL, NULL);
    SDL_RenderPresent(renderer);

    SDL_Event event;
    int quit = 0;
    while (!quit) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                quit = 1;
            }
        }
    }

    SDL_DestroyTexture(texture);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_FreeSurface(image);
    IMG_Quit();
    SDL_Quit();

    return EXIT_SUCCESS;
}
