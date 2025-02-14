#ifndef IA_PROOF_H
#define IA_PROOF_H

#include "../neuron/neuron.h"

#define LEARNING_RATE 0.01
#define NUM_EPOCHS 100000



//Sigmoid activation function.
double sigmoid(double x);


//Derivative of the sigmoid function.
double sigmoid_derivative(double x);

/**
 * Saves the model to a file.
 * @param layers Array of layers in the neural network.
 * @param num_layers Number of layers in the network.
 * @param filename Path to the file where the model will be saved.
 */
void save_model(Layer *layers, int num_layers, const char *filename);

/**
 * Loads the model from a file.
 * @param layers Array of layers in the neural network.
 * @param num_layers Number of layers in the network.
 * @param filename Path to the file where the model is saved.
 */
void load_model(Layer *layers, int num_layers, const char *filename);

/**
 * Forward propagation.
 * @param inputs Array of input values.
 * @param hidden_layer The hidden layer of the neural network.
 * @param output_layer The output layer of the neural network.
 * @param result Output array of the hidden layer activations.
 * @return Output from the forward propagation process.
 */
double forward(double inputs[2], Layer *hidden_layer,
Layer *output_layer, double *result);

/**
 * Backpropagation function.
 * @param inputs Array of input values.
 * @param expected Expected output value.
 * @param input_layer The input layer of the neural network.
 * @param output_layer The output layer of the neural network.
 * @param received The actual output value from the network.
 * @param result Array of the hidden layer activations.
 */
void backpropagation(double inputs[2], double expected,
Layer *input_layer, Layer *output_layer, double received, double *result);

/**
 * Predicts the output based on input values A and B.
 * @param layers Array of layers in the neural network.
 * @param A First input value.
 * @param B Second input value.
 */
void predict(Layer *layers, double A, double B, double *val);

//executable function of iaproof
int exe_iaproof(int argc, char *argv[], double *val) ;

#endif // IA_PROOF_H
