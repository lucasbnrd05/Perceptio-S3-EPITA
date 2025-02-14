#ifndef IALETTER_H
#define IALETTER_H

#include "../neuron/neuron.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <pthread.h>
#include <err.h>
#include "../image_binarization/binarization.h"


#define LEARNING 0.001
#define NUM_EPOCH 100
#define IMAGE_SIZE 1024 // 32 * 32
#define BAR_LENGTH 50
#define MAX_THREADS 8 // Maximum number of threads

// // Structure to hold thread data for training
// typedef struct {
//     Layer *hidden_layer;   // the hidden layer
//     Layer *output_layer;   // output layer
//     int **images;          // pointer of array of image
//     int *labels;           // pointer of array of label
//     int image_index;       // index of image
//     double *result;        // array for the result
// } ThreadData;

// Function to convert the cpord from image to an array of pixels
int ImageToArray_coord(const int coords[2][2], const char *imagePath, char *tab);

// Function to convert the sdl image to an array of pixels
int ImageToArray_SDL(SDL_Surface *image, char *tab);

// Function to convert the image to an array of pixels
void convertImageToArray(const char *imagePath, int *result);

// Load dataset from a file
void loadDatasetFromFile(const char *filename, int ***images, int ***labels, int *numImages);

// Save the model to a file
void save_model_ia(Layer *layers, int num_layers, const char *filename);

// Load the model for letters from a file
void load_model_ia(Layer *layers, int num_layers, const char *filename);

// Tanh activation function and its derivative
double tanh_activation(double x);
double tanh_derivative(double x);

double sigmoid_activation2(double x);
double sigmoid_derivative2(double x);

// ReLU activation function and its derivative
double relu(double x);
double relu_derivative(double x);

// Softmax function
void softmax(double input[], double output[], int size);

// Forward propagation function
int forward_ia(int *image, Layer *hidden_layer, Layer *output_layer, double *result);

// Backpropagation function
void backpropagation_ia(Layer *hidden_layer, Layer *output_layer, double *expected, double *result, int **images, int image_index);

// Function to display labels
void displayLabels(int **labels, int numImages);

// Function to get the label from a prediction
int getLabelFromPrediction(int *label);


// function for training images
void train_image(int *image, int *label, Layer *hidden_layer, Layer *output_layer, double *result);

// Main function
int exec_ia(int argc, char *argv[]);

#endif // IALETTER_H