#ifndef _NET_H_
#define _NET_H_

#include <iostream>
#include <vector>
#include <cstdint>
#include <initializer_list>
#include <cassert>
#include <stdexcept>
#include <random>
#include <cmath>
#include <fstream>
#include <ostream>

#include "utils.h"

class Net_t {

    friend std::ostream & operator<<(std::ostream &os, const Net_t &net);

    public:
        // Constructor:
        //  layers is a list, where each parameter indicates the number of neurons per layer.
        //  e.g. {2, 3, 1} -> 1 layer with 2 input neurons, 1 hidden layer with 3 neurons, 1 output layer with 1 neuron
        explicit Net_t(std::initializer_list<uint16_t> const &layers);

        explicit Net_t(std::string const &filename);

        explicit Net_t(std::uint16_t const &input_size, const std::vector<MatDouble_t>& weights);

        // Fits the Neural Network
        //  X_train: input dataset for training
        //  y_train: input labels for training
        //  lr: learning rate for gradient descent
        //  batch_size: how many examples of X_train takes the training at same time for feedforward and backpropagation    // 
        //  epochs: training iterations (each epoch, the whole X dataset is passed to the network)
        //  X_validation: used to feed the net to check the error of the model evaluating data that it does not see while training at the end of each epoch.
        //  X_validation: used to compare the outputs of X_validation and calculate the error.
        //  verbose: indicates if the errors obtained both for training and validations datasets must be printed in the stantard output at the end of each epoch.
        //  regularization_lambda: regularization parameter for L2 regularization.
        VecPair_t fit(
              MatDouble_t const &X_train
            , MatDouble_t const &y_train            
            , double const &lr
            , uint32_t const& batch_size
            , std::size_t const &epochs
            , MatDouble_t const &X_validation
            , MatDouble_t const &y_validation
            , uint8_t const &verbose = 0
            , double const &regularization_lambda = 0.0
        );

        // Evaluates the network: returns its error (mean squared error)
        //  X: input dataset
        //  y: input labels
        double evaluate(MatDouble_t const &X, MatDouble_t const &y);

        VecDouble_t predict(VecDouble_t const &X) const;

        // Saves the model weights and architecture in the specified file in CSV format.
        // Each line of the file represents a signal Sj that contains:
        //  LayerNumber NumberOfParamsInTheSignal,wij,wi+1j,wi+nj
        void save_model(std::string const &filename) const;

        // Load the model with format specified by the function save_model(std::string const &filename)
        void load_model(std::string const &filename);

        // Prints in the standard output the net architecture with the following format:
        // Architecture: { 4 2 3 1}
        // In that example, the net input layer has 4 neurons, 2 hidden layers with 2 and 3 neurones each, and 1 neuron in the output layer
        void printArchitecture() const;

    private:
        // Neural Network architecture
        std::vector<MatDouble_t> m_layers;

        // Size of the input layer
        std::uint16_t input_size;
        
        // ######################## BEGIN AUXILIAR METHODS ########################
        // Multiply Xi*Wij and add them to obtain the signal Sj that enters a neuron
        VecDouble_t multiplyT(VecDouble_t const &x, MatDouble_t const &W) const;

        // Sigmoid function
        double sigmoid(double x) const;

        // Sigmoid derivative function: 
        //  sigmoid(value) * (1 -  sigmoid(value))
        //  x: expects to be sigmoid(value))
        double sigmoid_derivative(double x) const;

        // Calculates the sigmoid function for every value of the VecDouble
        VecDouble_t sigmoid(VecDouble_t const &v) const;

        void backpropagation_without_update_weights (
              std::vector<MatDouble_t>& gradients
            , std::vector<VecDouble_t> const &outputs
            , VecDouble_t const &Xi
            , VecDouble_t const &yi
            , double const &regularization_lambda
        );

        // Feeds the network forward, applying the activation function to the corresponding signals Sj
        //  Xi: input example
        //  outputs: results of activation(Signal j) for all neurons and layers.
        void feedforward_train(std::vector<VecDouble_t>& outputs, VecDouble_t const &Xi);

        // Feeds the network forward, applying the activation function to the corresponding signals Sj
        //  x: input example
        VecDouble_t feedforward(VecDouble_t const &x) const;

        // Calculates delta for last layer (output layer)
        //  a: vector of outputs of the last layer (output layer)
        //  y: vector of labels (expected outputs)
        VecDouble_t calculate_last_delta(VecDouble_t const &a, VecDouble_t const &y);

        // Calculates delta for hidden layers
        //  a: vector of signals of layer l-1
        //  last_layer: weights of layer l
        //  deltas: vector of deltas of the layer l
        VecDouble_t calculate_hidden_delta(VecDouble_t const &a, MatDouble_t const &last_layer, VecDouble_t const &deltas);

        // Updates weights in backpropagation
        //  gradients: matrix of gradients (1 per weight of the layer of the Neural Network)
        //  layer: matrix of gradients (1 per weight of the layer of the Neural Network)
        //  a: outputs of the previous layer
        //  delta: deltas for the given layer
        //  lambda: L2 regularization lambda 
        void calculate_gradients(MatDouble_t &gradients, MatDouble_t &layer, VecDouble_t const &a, VecDouble_t const &delta, double const &lambda);

        // Updates weights in backpropagation
        //  gradients: matrix of gradients (1 per weight in the Neural Network)
        //  learning_rate: learning rate used for training
        void update_weights(std::vector<MatDouble_t> const &gradients, double const &lr, uint32_t const& batch_size);         

        // Check that the size of each example in X corresponds to the input size of the net and the size of each example in y 
        // corresponds to the output size of the net.
        // Tag is just used for the message given by the exception.
        void checkDatasetSize(MatDouble_t const &X, MatDouble_t const &y, std::string const &tag) const;        

        // ######################## END AUXILIAR METHODS ########################        

};

#endif