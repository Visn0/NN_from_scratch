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

        // Fits the Neural Network
        //  X: input dataset
        //  y: input labels
        //  epochs: training iterations (each epoch, the whole X dataset is passed to the network)
        VecPair_t fit(
              MatDouble_t const &X_train
            , MatDouble_t const &y_train            
            , double const &lr
            , std::size_t const &epochs
            , MatDouble_t const &X_test
            , MatDouble_t const &y_test
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

        void load_model(std::string const &filename);

    private:
        // Neural Network architecture
        std::vector<MatDouble_t> m_layers;
        
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

        // Feeds the network forward, applying the activation function to the corresponding signals Sj
        //  x: input dataset
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
        //  layer: layers whose weights must be updated
        //  a: vector of results of activation functions from previous layer [(X_i)^(l-1))
        //  delta: vector of deltas corresponding to each signal Sj of the given layer
        //  learning_rate: learning rate used for training
        void update_weights(MatDouble_t &layer, VecDouble_t const &a, VecDouble_t const &delta, double const &learning_rate, double const &lambda);            

        // Returns true if (a == b), throws Exception otherwise.
        bool checksize(int a, int b);

        // ######################## END AUXILIAR METHODS ########################        

};

#endif