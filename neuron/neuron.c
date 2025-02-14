#include <stdio.h>
#include <stdlib.h>
#include <math.h>


#define LEARNING_RATE 0.01 // Learning rate

// Structure of a neuron with its weights, bias, and output
typedef struct {
    double *weights; // Pointer to an array of weights for the connections to this neuron
    int num_weights; // Number of weights associated with this neuron (should equal the number of inputs)
    double bias; // Bias term for the neuron, used to shift the activation function
    double output; // Output value of the neuron after applying the activation function
    double delta; // Error gradient for backpropagation, representing the sensitivity of the neuron's output to the error
} Neuron;

// Structure of a layer -> multiple neurons
typedef struct {
    Neuron *neurons; // Pointer to an array of Neuron structures, representing the neurons in this layer
    int num_neurons; // Number of neurons in this layer
} Layer;



// Creation of a neuron (random initialization)
Neuron create_neuron(int num_inputs) {
    Neuron neuron;
    neuron.num_weights = num_inputs;
    neuron.weights = malloc(num_inputs * sizeof(double));
    for (int i = 0; i < num_inputs; i++) {
        neuron.weights[i] = ((double)rand() / RAND_MAX) * 2 - 1;
        // Random between -1 and 1
    }
    neuron.bias = ((double)rand() / RAND_MAX) * 2 - 1; // Random bias
    neuron.output = 0;
    neuron.delta = 0;
    return neuron;
}

// Creating a layer with number of neurons
Layer create_layer(int num_inputs, int num_neurons) {
    Layer layer;
    layer.neurons = malloc(num_neurons * sizeof(Neuron));
    layer.num_neurons = num_neurons;
    for (int i = 0; i < num_neurons; i++) {
        layer.neurons[i] = create_neuron(num_inputs);
    }
    return layer;
}
