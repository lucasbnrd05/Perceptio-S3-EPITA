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



// typedef struct {
//     Layer *hidden_layer;
//     Layer *output_layer;
//     int **images;
//     int *labels;
//     int image_index;
//     double *result;
// } ThreadData;
/*
HOW TO COMPILE:
    Go to the level -1, so the directory above
    Run the following command: gcc letter_ia/ia_letter.c neuron/neuron.c -o letter_ia/ia_letter image_binarization/binarization.c -lm -lSDL2 -lSDL2_image -lpthread -Wall -Werror
    Then run ./letter_ia/ia_letter to start the program
*/

//coord to array
int ImageToArray_coord(const int coords[2][2], const char *imagePath, char *tab) 
{
    SDL_Surface *image = IMG_Load(imagePath);
    if (image == NULL) 
    {
        errx(1, "Error: Unable to load image: %s", imagePath);
    }

    int width = image->w;
    int height = image->h;
    int x1 = coords[0][0]; // Top-left x
    int y1 = coords[0][1]; // Top-left y
    int x2 = coords[1][0]; // Bottom-right x
    int y2 = coords[1][1]; // Bottom-right y

    if (x1 < 0 || y1 < 0 || x2 >= width || y2 >= height || x1 >= x2 || y1 >= y2) 
    {
        SDL_FreeSurface(image);
        errx(1, "Error: Invalid coordinates.");
    }

    for (int i = 0; i < 1024; i++) 
    {
        tab[i] = '0';
    }
    /*convertToGrayscale(image);
    increaseGrayContrast(image);
    apply_gaussian_blur(image);
    sauvolaThresholding(image);
    for(int i = 0; i<3 ; i++){
        simpleThinning(image, 6, 255);
    }
    simpleDenoising(image);*/
    for (int y = y1; y < y2; y++) 
    {
        for (int x = x1; x < x2; x++) 
        {
            Uint32 pixel = ((Uint32 *)image->pixels)[y * width + x];
            Uint8 r, g, b;
            SDL_GetRGB(pixel, image->format, &r, &g, &b);
            Uint8 brightness = (Uint8)(0.299 * r + 0.587 * g + 0.114 * b);
            tab[(y - y1) * 32 + (x - x1)] = (brightness < 128) ? '1' : '0';  // value to array
        }
    }

    tab[1024] = '\0';  //terminate the string

    SDL_FreeSurface(image);  // Free image
    return 0;  
}


//image to array
int ImageToArray_SDL(SDL_Surface *image, char *tab) 
{
    if (image == NULL) 
    {
        errx(1, "Error: Received a NULL image.");
    }
    int width = image->w;
    int height = image->h;
    if (width > 32 || height > 32) 
    {
        errx(1, "Error: Image too large.");
    }
    for (int i = 0; i < 1024; i++) {
        tab[i] = '0';
    }
    /*convertToGrayscale(image);
    increaseGrayContrast(image);
    apply_gaussian_blur(image);
    sauvolaThresholding(image);
    for(int i = 0; i<3 ; i++){
        simpleThinning(image, 6, 255);
    }
    simpleDenoising(image);*/
    // Fill the array based on the image pixels
    for (int y = 0; y < height; y++) 
    {
        for (int x = 0; x < width; x++) 
        {
            Uint32 pixel = ((Uint32 *)image->pixels)[y * width + x];
            Uint8 r, g, b;
            SDL_GetRGB(pixel, image->format, &r, &g, &b);
            Uint8 brightness = (Uint8)(0.299 * r + 0.587 * g + 0.114 * b);
            tab[y * 32 + x] = (brightness < 128) ? '1' : '0';  
        }
    }
    tab[1024] = '\0';  //terminate the string
    return 0; 
}

// function to convert image to array
void convertImageToArray(const char *imagePath, int *result) 
{
    for (int y = 0; y < IMAGE_SIZE; y++) 
    {
        result[y] = (imagePath[y] == '1') ? 1 : 0;
    }
}
//load data
void loadDatasetFromFile(const char *filename, int ***images, int ***labels, int *numImages) 
{
    FILE *file = fopen(filename, "r");
    if (!file) 
    {
        fprintf(stderr, "Error opening file : %s\n", filename);
        return;
    }
    // read number of image
    if (fscanf(file, "%d\n", numImages) != 1) 
    {
        fprintf(stderr, "error reading image.\n");
        fclose(file);
        return;
    }
    *images = (int **)malloc(*numImages * sizeof(int *));
    *labels = (int **)malloc(*numImages * sizeof(int *));
    if (!*images || !*labels) 
    {
        fprintf(stderr, "error malloc load data\n");
        fclose(file);
        return;
    }

    char line[8192]; 
    for (int i = 0; i < *numImages; i++) 
    {
        if (!fgets(line, sizeof(line), file)) 
        {
            fprintf(stderr, "error reading image 2 %d.\n", i);
            break;
        }
        // find ; and change to '\0'
        char *semicolon = strchr(line, ';');
        if (semicolon) 
        {
            *semicolon = '\0'; 
            semicolon++; 
        } 
        else 
        {
            fprintf(stderr, "no ; for the image  %d. line: %s\n", i, line);
            continue;
        }
        (*images)[i] = (int *)malloc(IMAGE_SIZE * sizeof(int));
        if (!(*images)[i]) 
        {
            fprintf(stderr, "error malloc\n");
            continue;
        }
        for (int pixelIndex = 0; pixelIndex < IMAGE_SIZE; pixelIndex++) 
        {
            if (pixelIndex < (int)strlen(line)) 
            { 
                (*images)[i][pixelIndex] = (line[pixelIndex] == '1') ? 1 : 0; // 1 if pixel is black else 0
            } 
            else 
            {
                (*images)[i][pixelIndex] = 0; 
            }
        }
        // for the label
        (*labels)[i] = (int *)calloc(26, sizeof(int)); 
        if (!(*labels)[i]) 
        {
            fprintf(stderr, "error malloc label\n");
            free((*images)[i]); 
            continue;
        }
        // set the label
        int labelIndex = atoi(semicolon); 
        if (labelIndex >= 1 && labelIndex <= 26) 
        { 
            (*labels)[i][labelIndex - 1] = 1; 
        } 
        else 
        {
            fprintf(stderr, "invalid label : %d\n", labelIndex);
        }
    }
    fclose(file);
}

// save the model
void save_model_ia(Layer *layers, int num_layers, const char *filename) 
{
    FILE *file = fopen(filename, "w");
    for (int i = 0; i < num_layers; i++) 
    {
        for (int j = 0; j < layers[i].num_neurons; j++) 
        {
            for (int k = 0; k < (i == 0 ? IMAGE_SIZE : layers[i - 1].num_neurons); k++) 
            {
                fprintf(file, "%lf\n", layers[i].neurons[j].weights[k]);
            }
            fprintf(file, "%lf\n", layers[i].neurons[j].bias);
        }
    }
    fclose(file);
}

// load the model for letters
void load_model_ia(Layer *layers, int num_layers, const char *filename) 
{
    FILE *file = fopen(filename, "r");
    if (file == NULL) 
    {
        layers[0] = create_layer(IMAGE_SIZE, 392); // Input layer
        layers[1] = create_layer(392, 26);  // Output layer
        return;
    }
    for (int i = 0; i < num_layers; i++) 
    {
        for (int j = 0; j < layers[i].num_neurons; j++) 
        {
            for (int k = 0; k < (i == 0 ? IMAGE_SIZE : layers[i - 1].num_neurons); k++) 
            {
                fscanf(file, "%lf", &layers[i].neurons[j].weights[k]);
            }
            fscanf(file, "%lf", &layers[i].neurons[j].bias);
        }
    }
    fclose(file);
}

double sigmoid_activation2(double x) {
    return 1.0 / (1.0 + exp(-x));
}

double sigmoid_derivative2(double x) {
    return x * (1.0 - x);
}


//  tanh and the derivative
double tanh_activation(double x) 
{
    return tanh(x);
}

double tanh_derivative(double x) 
{
    return 1.0 - tanh(x) * tanh(x);
}

//  ReLU and the derivative
double relu(double x) 
{
    return fmax(0.0, x);
}

double relu_derivative(double x) 
{
    return x > 0 ? 1 : 0;
}
//softmax
void softmax(double input[], double output[], int size) 
{
    if (size <= 0 || input == NULL || output == NULL) 
    {
        fprintf(stderr, "Invalid input or output array.\n");
        return;
    }
    double max = input[0];
    for (int i = 1; i < size; i++) 
    {
        if (input[i] > max) 
        {
            max = input[i];
        }
    }
    double sum = 0.0;
    for (int i = 0; i < size; i++) 
    {
        output[i] = exp(input[i] - max);
        sum += output[i];
    }
    for (int i = 0; i < size; i++) 
    {
        output[i] /= sum;
    }
}

// Forward propagation
int forward_ia(int *image, Layer *hidden_layer, Layer *output_layer, double *result) 
{

    for (int neuron_index = 0; neuron_index < hidden_layer->num_neurons; neuron_index++) 
    {
        hidden_layer->neurons[neuron_index].output = hidden_layer->neurons[neuron_index].bias;
        for (int weight_index = 0; weight_index < IMAGE_SIZE; weight_index++) 
        {
            hidden_layer->neurons[neuron_index].output += hidden_layer->neurons[neuron_index].weights[weight_index] * image[weight_index];
        }
        result[neuron_index] = relu(hidden_layer->neurons[neuron_index].output); // Utiliser tanh
    }
    double raw_output[output_layer->num_neurons];
    for (int neuron_index = 0; neuron_index < output_layer->num_neurons; neuron_index++) 
    {
        output_layer->neurons[neuron_index].output = output_layer->neurons[neuron_index].bias;
        for (int weight_index = 0; weight_index < hidden_layer->num_neurons; weight_index++) 
        {
            output_layer->neurons[neuron_index].output += output_layer->neurons[neuron_index].weights[weight_index] * result[weight_index];
        }
        raw_output[neuron_index] = relu(output_layer->neurons[neuron_index].output);
    }
    double softmax_output[output_layer->num_neurons];
    softmax(raw_output, softmax_output, output_layer->num_neurons);
    for (int i = 0; i < output_layer->num_neurons; i++) 
    {
        result[i] = softmax_output[i];
    }
    int max_index = 0;
    for (int i = 1; i < output_layer->num_neurons; i++) 
    {
        if (softmax_output[i] > softmax_output[max_index]) 
        {
            max_index = i;
        }
    }
    return max_index;
}

//function for the backpropagation
void backpropagation_ia(Layer *hidden_layer, Layer *output_layer, double *expected, double *result, int **images, int image_index) 
{
    // error exit
    for (int i = 0; i < output_layer->num_neurons; i++) 
    {
        double error = expected[i] - result[i];
        output_layer->neurons[i].delta = error * relu_derivative(result[i]); 
    }
    // error first layer
    for (int i = 0; i < hidden_layer->num_neurons; i++) 
    {
        double error = 0.0;
        for (int j = 0; j < output_layer->num_neurons; j++) 
        {
            error += output_layer->neurons[j].delta * output_layer->neurons[j].weights[i];
        }
        hidden_layer->neurons[i].delta = error * relu_derivative(hidden_layer->neurons[i].output); // Utiliser tanh
    }
    for (int i = 0; i < output_layer->num_neurons; i++) 
    {
        for (int j = 0; j < hidden_layer->num_neurons; j++) 
        {
            output_layer->neurons[i].weights[j] += LEARNING * output_layer->neurons[i].delta * result[j];
        }
        output_layer->neurons[i].bias += LEARNING * output_layer->neurons[i].delta;
    }
    for (int i = 0; i < hidden_layer->num_neurons; i++) 
    {
        for (int j = 0; j < IMAGE_SIZE; j++) 
        {
            hidden_layer->neurons[i].weights[j] += LEARNING * hidden_layer->neurons[i].delta * images[image_index][j];
        }
        hidden_layer->neurons[i].bias += LEARNING * hidden_layer->neurons[i].delta;
    }
}

//function to display labels
void displayLabels(int **labels, int numImages) 
{
    for (int i = 0; i < numImages; i++) 
    {
        printf("Label for image %d: ", i + 1);
        for (int j = 0; j < 26; j++) 
        {
            printf("%d ", labels[i][j]);
        }
        printf("\n");
    }
}

//function to take the result
int getLabelFromPrediction(int *label) 
{
    for (int j = 0; j < 26; j++) 
    {
        if (label[j] == 1) 
        {
            return j; 
        }
    }
    return -1; 
}
int getIndexOfOne(int *labels, int size) {
    for (int i = 0; i < size; i++) {
        if (labels[i] == 1) {
            return i; // return index of the letter
        }
    }
    return -1; // error
}



// Function thread
void train_image(int *image, int *label, Layer *hidden_layer, Layer *output_layer, double *result) {
    double expected[26] = {0};
    for (int j = 0; j < 26; j++) {
        expected[j] = (double)label[j];
    }
    // Forward propagation
   int predicted_index = forward_ia(image, hidden_layer, output_layer, result);
    
        
    printf("\nThe predicted letter is \033[32m%c\033[0m\n", 'A' + (predicted_index));

    // Backpropagation
    backpropagation_ia(hidden_layer, output_layer, expected, result, &image, 0);
}
int exec_ia(int argc, char *argv[]) 
{
    if (argc < 2) 
    {
        fprintf(stderr, "Please provide the learning or prediction argument: \n-l for learning \n-p <image> for prediction\n");
        return 1;
    }

    Layer layers[2];
    layers[0] = create_layer(IMAGE_SIZE, 512);  // Input layer
    layers[1] = create_layer(512, 26);   // Output layer

    load_model_ia(layers, 2, "source_file/data_base/modelletter.txt");

    int **images; // Array of pointers to images
    int **labels; // Array of pointers to labels
    int numImages;

    if (strcmp(argv[1], "-l") == 0) 
    {
        loadDatasetFromFile("source_file/data_base/dataset_labels.txt", &images, &labels, &numImages);
        for (int ref = 0; ref < NUM_EPOCH; ref++) 
        {

            for (int i = 0; i < numImages; i++) 
            {
                for (int j = 0; j < 1024; j++) 
                {
                    if (images[i][j] == 1) 
                    {
                        // Change text color to green for '1'
                        printf("\033[32m%c\033[0m", '1');
                    } 
                    else 
                    {
                        // Keep text color default for other characters
                        putchar('0' );
                    }

                    // Print a newline every 32 characters
                    if ((j + 1) % 32 == 0) 
                    {
                        putchar('\n');
                    }
                }
                double *result = (double *)malloc(layers[0].num_neurons * sizeof(double));
                // Entraînement de l'image
                train_image(images[i], labels[i], &layers[0], &layers[1], result);
                free(result);
            }
            

            // Progress bar update
            int progress = (ref + 1) * BAR_LENGTH / NUM_EPOCH;
            printf("\rProgress: [");
            for (int j = 0; j < BAR_LENGTH; j++) 
            {
                if (j < progress) 
                {
                    printf("=");
                } 
                else 
                {
                    printf(" ");
                }
            }
            printf("] %d%%", (ref + 1) * 100 / NUM_EPOCH);
            fflush(stdout);
        }
        printf("\n");
        // Save model and free resources
        save_model_ia(layers, 2, "source_file/data_base/modelletter.txt");
        for (int i = 0; i < numImages; i++) 
        {
            free(images[i]);
            free(labels[i]);
        }
        free(images);
        free(labels);
    }
    //predict
    else if (strcmp(argv[1], "-p") == 0) 
    {
        if (argc < 3) 
        {
            fprintf(stderr, "Please provide image.\n");
            return 1;
        }
        int num_neurons = 1024;
        int image[num_neurons]; 

        convertImageToArray(argv[2], image);
        double result[layers[0].num_neurons]; 
        int predicted_index = forward_ia(image, &layers[0], &layers[1], result);
        
        printf("\nThe letter predicted is \033[32m%c\033[0m\n", 'A' + (predicted_index));
        return predicted_index;
    }
//free
    for (int i = 0; i < 2; i++) 
    {
        for (int j = 0; j < layers[i].num_neurons; j++) 
        {
            free(layers[i].neurons[j].weights);
        }
        free(layers[i].neurons);
    }

    return 0;
}