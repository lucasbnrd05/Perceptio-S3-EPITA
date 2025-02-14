#ifndef NEURON_H
#define NEURON_H

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define LEARNING_RATE 0.01 // Learning rate

// Structure neuron
typedef struct {
    double *weights;
    int num_weights;
    double bias;
    double output;
    double delta; // Error
} Neuron;

// Structure layer
typedef struct {
    Neuron *neurons;
    int num_neurons;
} Layer;

//create a neuron
Neuron create_neuron(int num_inputs);

//create a layer
Layer create_layer(int num_inputs, int num_neurons);

#endif // NEURON_H