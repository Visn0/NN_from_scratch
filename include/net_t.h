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

#include "globals.h"

class Net_t {
    
    public:
        // Constructor:
        //  layers is a list, where each parameter indicates the number of neurons per layer.
        //  e.g. {2, 3, 1} -> 1 layer with 2 input neurons, 1 hidden layer with 3 neurons, 1 output layer with 1 neuron
        explicit Net_t(std::initializer_list<uint16_t> const &layers);

        // Fits the Neural Network
        //  X: input dataset
        //  y: input labels
        void fit(MatDouble_t const &X, MatDouble_t const &y, double const &learning_rate);

        // Evaluates the network: returns its error (mean squared error)
        //  X: input dataset
        //  y: input labels
        double evaluate(MatDouble_t const &X, MatDouble_t const &y);

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
        void update_weights(MatDouble_t &layer, VecDouble_t const &a, VecDouble_t const &delta, double const &learning_rate);    

        // Generates a random number between [min, max]
        double randDouble(double min, double max);

        // Fills the vector vec with random numbers between [min, max]
        void fillVectorRandom(VecDouble_t &vec, double min, double max);

        // Returns true if (a == b), throws Exception otherwise.
        bool checksize(int a, int b);

        // ######################## END AUXILIAR METHODS ########################        

};

#endif