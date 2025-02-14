#include "../neuron/neuron.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>



#define LEARNING_RATE 0.01
#define NUM_EPOCHS 100000

/*
HOW TO COMPILE:
    Go to the level -1, so the directory above
    Run the following command: gcc xor/ia_proof.c
    neuron/neuron.c -o xor/ia_proof -lm -Wall -Wextra
    Then run ./xor/ia_proof to start the program
*/

// Sigmoid activation function
double sigmoid(double x) {
    return 1.0 / (1.0 + exp(-x));
}

// Derivative of the sigmoid function
double sigmoid_derivative(double x) {
    double s = sigmoid(x);
    return s * (1 - s);
}

void save_model(Layer *layers, int num_layers, const char *filename) {
    FILE *file = fopen(filename, "w");
    for (int i = 0; i < num_layers; i++) {
        for (int j = 0; j < layers[i].num_neurons; j++) {
            for (int k = 0; k < (i == 0 ? 2 : layers[i - 1].num_neurons);
            k++)
            {
                fprintf(file, "%lf\n", layers[i].neurons[j].weights[k]);
            }
            fprintf(file, "%lf\n", layers[i].neurons[j].bias);
        }
    }
    fclose(file);
}

void load_model(Layer *layers, int num_layers, const char *filename) {
    FILE *file = fopen(filename, "r");
    // If the file does not exist
    if (file == NULL) {
            layers[0] = create_layer(2, 4);
            // Input layer with 4 hidden neurons
            layers[1] = create_layer(4, 1);
            // Output layer with 1 neuron
            return;
    }
    // Otherwise
    for (int i = 0; i < num_layers; i++) {
        for (int j = 0; j < layers[i].num_neurons; j++) {
            for (int k = 0; k < (i == 0 ? 2 : layers[i - 1].num_neurons);
            k++)
            {
                fscanf(file, "%lf", &layers[i].neurons[j].weights[k]);
            }
            fscanf(file, "%lf", &layers[i].neurons[j].bias);
        }
    }
    fclose(file);
}

// Forward propagation
double forward(double inputs[2], Layer *hidden_layer, Layer *output_layer,
double *result) {
    // Calculate propagation in the hidden layer
    for(int neuron_index = 0; neuron_index < hidden_layer->num_neurons;
    neuron_index++)
    {
        hidden_layer->neurons[neuron_index].output =\
        hidden_layer->neurons[neuron_index].bias;  // Correct access

        for(int weight_index = 0; weight_index < 2; weight_index++) {
            // Use 2 as we have 2 inputs
            hidden_layer->neurons[neuron_index].output += \
            hidden_layer->neurons[neuron_index].weights[weight_index] * \
            inputs[weight_index];
        }
        result[neuron_index] = \
        sigmoid(hidden_layer->neurons[neuron_index].output);
        // Correct access
    }

    // Calculate propagation in the output layer
    double out;  // Type correction
    for(int neuron_index = 0; neuron_index < output_layer->num_neurons;
    neuron_index++)
    {
        output_layer->neurons[neuron_index].output =\
         output_layer->neurons[neuron_index].bias;  // Correct access

        for(int weight_index = 0; weight_index < hidden_layer->num_neurons;
        weight_index++)
        {  // Use the number of neurons in the hidden layer
            output_layer->neurons[neuron_index].output +=\
             output_layer->neurons[neuron_index].weights[weight_index] * \
             result[weight_index];
        }
        out = sigmoid(output_layer->neurons[neuron_index].output);
        // Store the final result
    }
    return out;  // Directly return the result
}

void backpropagation(double inputs[2], double expected, Layer *input_layer,
Layer *output_layer, double received, double *result){

    // Calculate the error and gradient
    for (int i = 0; i < output_layer->num_neurons; i++) {
        double error = expected - received;
        output_layer->neurons[i].delta = error * sigmoid_derivative(received);
    }

    // Propagate the error to the previous layer
    for (int i = 0; i < input_layer->num_neurons; i++) {
        double error = 0;
        for (int j = 0; j < output_layer->num_neurons; j++) {
            error += output_layer->neurons[j].delta * \
            output_layer->neurons[j].weights[i];
        }
        input_layer->neurons[i].delta = error * \
        sigmoid_derivative(result[i]);
    }

    // Update weights and biases
    for (int i = 0; i < output_layer->num_neurons; i++) {
        for (int j = 0; j < input_layer->num_neurons; j++) {
            output_layer->neurons[i].weights[j] += LEARNING_RATE * \
            output_layer->neurons[i].delta * result[j];
        }
        output_layer->neurons[i].bias += LEARNING_RATE * \
        output_layer->neurons[i].delta;
    }

    for (int i = 0; i < input_layer->num_neurons; i++) {
        for (int j = 0; j < 2; j++) {
            input_layer->neurons[i].weights[j] += LEARNING_RATE * \
            input_layer->neurons[i].delta * inputs[j];
        }
        input_layer->neurons[i].bias += LEARNING_RATE * \
        input_layer->neurons[i].delta;
    }
}

// Function to predict based on A and B and return the result
void predict(Layer *layers, double A, double B, double *val) {
    double inputs[2] = {A, B};
    double result[4];  // Adapt to the hidden layer
    double res = forward(inputs, &layers[0], &layers[1], result);
    *val = res;
    printf("Prediction for A = %lf, B = %lf: %lf\n", A, B, res);
    // If result is close to 1 == 1, otherwise it equals 0
}

int exe_iaproof(int argc, char *argv[], double *val) {

    Layer layers[2];
    layers[0] = create_layer(2, 4);
    // Input layer with 4 hidden neurons
    layers[1] = create_layer(4, 1);
    // Output layer with 1 neuron

    load_model(layers, 2, "source_file/data_base/modelxor.txt");

    if (argc < 2) {
        fprintf(stderr,
        "Please provide the learning or prediction argument:\
        \n-l for learning\n-p A B for prediction\n");
        return 1;
    }

    if (strcmp(argv[1], "-l") == 0) {
        // Learning
        // Training data
        double training_inputs[4][2] = {
            {0, 0},  // 1
            {0, 1},  // 0
            {1, 0},  // 0
            {1, 1}   // 1
        };

        double training_outputs[4] = {1, 0, 0, 1};
        // Outputs for XNOR

        for(int ref = 0; ref < NUM_EPOCHS; ref++) {
            double error = 0;

            for(int i = 0; i < 4; i++) {
                double result[4];  // Declare the result array

                double e = forward(training_inputs[i], &layers[0],
                &layers[1], result);  // Pass result directly
                //printf("Forward propagation: %lf\n", e);

                error += fabs(training_outputs[i] - e);
                //printf("Error: %lf\n", error);

                backpropagation(training_inputs[i], training_outputs[i],
                &layers[0], &layers[1], e, result);  // Pass result directly
            }
        }
        save_model(layers, 2, "source_file/data_base/modelxor.txt");
        *val = -1;

    }
    else if (strcmp(argv[1], "-p") == 0) {
        if (argc < 4) {
            fprintf(stderr, "Please provide A and B for prediction.\n");
            return 1;
        }

        double A = atof(argv[2]);
        double B = atof(argv[3]);

        predict(layers, A, B, val);
    }
    // Free memory
    for (int i = 0; i < 2; i++) {
        for (int j = 0; j < layers[i].num_neurons; j++) {
            free(layers[i].neurons[j].weights);
        }
        free(layers[i].neurons);
    }
    return 0;
}
