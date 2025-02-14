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
#include "net_t.h"
#include "utils.h"

Net_t::Net_t(std::initializer_list<uint16_t> const &layers)
{
    if (layers.size() < 2)
        throw std::out_of_range("Net_t");

    auto input_size = *layers.begin();
    for (auto it = layers.begin() + 1; it != layers.end(); ++it)
    {
        MatDouble_t layer_w(*it);
        for (auto &v : layer_w)
        {
            v.resize(input_size + 1); // +threshold
            fillVectorRandom(v, -10.0, 10.0);
        }
        m_layers.push_back(layer_w);
        input_size = *it;
    }
}

Net_t::Net_t(std::string const &filename) {
    load_model(filename);
}

VecPair_t Net_t::fit(
      MatDouble_t const &X_train
    , MatDouble_t const &y_train
    , double const &lr
    , std::size_t const &epochs
    , MatDouble_t const &X_test
    , MatDouble_t const &y_test
    , uint8_t const &verbose
    , double const &regularization_lambda
)
{
    // history[epoch] = pair(train_error, test_error)
    VecPair_t history(epochs);

    for (size_t epoch = 0; epoch < epochs; ++epoch)
    {
        print("\n####################################################\n");
        print("EPOCH:", epoch, "\n");
        print("####################################################\n");
        for (size_t i{0}; i < X_train.size(); ++i) // iterations of train
        {
            std::vector<std::pair<VecDouble_t,  // raw output (z)
                                    VecDouble_t>> // output through activation function (a)
                outputs;

            print("\n#################\n");
            print("Iteration", i, "\n");
            print("~~~~~~~~~~~~~~\n");

            print("NETWORK \n");
            for (size_t wi{0}; wi < m_layers.size(); ++wi)
            {
                print(m_layers[wi]);
            }

            // feedforward
            print("\n~~~~~~~~~~~~~~\n");
            print("FORWARD \n");
            print("~~~~~~~~~~~~~~\n");

            VecDouble_t result(X_train[i]);
            print("Input \t\t", result);
            print("Expected \t", y_train[i]);

            for (size_t wi{0}; wi < m_layers.size(); ++wi)
            {
                result.resize(result.size() + 1);
                std::copy(result.rbegin() + 1, result.rend(), result.rbegin());
                result[0] = 1.0;

                VecDouble_t z = multiplyT(result, m_layers[wi]);
                result = sigmoid(z);
                outputs.push_back(std::make_pair(z, result));

                print("Raw", wi, "\t\t", z);
                print("Output", wi, "\t", result);
            }

            print("\n~~~~~~~~~~~~~~\n");
            print("BACKPROPAGATION \n");
            print("~~~~~~~~~~~~~~\n");
            int L = m_layers.size() - 1;
            VecDouble_t z = outputs[L].first;
            VecDouble_t a = outputs[L].second;

            VecDouble_t delta = calculate_last_delta(a, y_train[i]);
            print("Delta", L, "\t\t", delta);

            // save last layer to use the original weights in next layer
            MatDouble_t last_layer(m_layers[L].size());
            for (std::size_t wi = 0; wi < m_layers[L].size(); ++wi)
            {
                VecDouble_t w(m_layers[L][wi]);
                last_layer[wi] = w;
            }
            update_weights(m_layers[L], outputs[L - 1].second, delta, lr, regularization_lambda);

            for (int l = L - 1; l > -1; --l)
            {
                z = outputs[l].first;
                a = outputs[l].second;
                delta = calculate_hidden_delta(a, last_layer, delta);
                print("Delta", l, "\t\t", delta);

                last_layer = MatDouble_t(m_layers[l].size());
                for (std::size_t wi = 0; wi < m_layers[l].size(); ++wi)
                {
                    VecDouble_t w(m_layers[l][wi]);
                    last_layer[wi] = w;
                }
                if (l == 0)
                {
                    update_weights(m_layers[l], X_train[i], delta, lr, regularization_lambda);
                }
                else
                {
                    update_weights(m_layers[l], outputs[l - 1].second, delta, lr, regularization_lambda);
                }
            }                       
        }
    
        const double train_error = evaluate(X_train, y_train);
        const double test_error  = evaluate(X_test, y_test);
        history[epoch] = std::make_pair( train_error, test_error ); 

        switch (verbose)
        {
            case 1:
                std::cout << "Epoch " << std::to_string(epoch);
                std::cout << ": train_error=" << std::to_string(train_error);
                std::cout << "\ttest_error=" << std::to_string(test_error);
                std::cout << std::endl;
                break;
            
            default: break;            
        }
    }

    return history;
}

double Net_t::evaluate(MatDouble_t const &X, MatDouble_t const &y)
{
    assert(X.size() == y.size());

    double error{0};
    for (std::size_t i{0}; i < y.size(); ++i)
    {
        auto h = feedforward(X[i]);
        for (std::size_t j{0}; j < h.size(); ++j)
        {
            double e = (h[j] - y[i][j]) * (h[j] - y[i][j]);
            error += e;
        }
    }

    error /= (y[0].size() * y.size());
    return error;
}

VecDouble_t Net_t::predict(VecDouble_t const &X) const {
    // MatDouble_t results;

    // for(const auto& v: X) 
    // {
    //     results.push_back( feedforward(v) );
    // }

    return feedforward(X);
}

void Net_t::save_model(std::string const &filename) const{
    std::ofstream file;
    file.open(filename, std::ios::out);

    for(std::size_t layer = 0; layer < m_layers.size(); ++layer) {        
        for(std::size_t signal_j = 0; signal_j < m_layers[layer].size(); ++signal_j) {
            std::string signal_str{ std::to_string(signal_j) };            

            auto* signal_ref = &m_layers[layer][signal_j];

            file << layer; // layer numbner
            file << "," <<  signal_ref->size(); // Number of parameters in the signal Sj
            
            for(std::size_t weight = 0; weight < signal_ref->size(); ++weight) {                                                
                file << "," << (*signal_ref)[weight]; // weigth ij
            }

            file << std::endl;
        }        
    }

    file.close();
    std::cout << "Model saved correctly: " << filename << std::endl;
    // std::cout << "---------SAVE MODEL" << std::endl;
    // std::cout << *this << std::endl << std::endl;
}

void Net_t::load_model(std::string const &filename) {    
    m_layers.clear();
    
    std::ifstream file;
    file.open(filename);

    if(!file.is_open())
    {
        throw std::runtime_error("[EXCEPTION]: File not found : " + filename);
    }    

    std::string line;                
    std::istringstream sline;       
    
    std::string prevLayer;
    std::string value;

    MatDouble_t layer;
    while(getline(file, line))
    {                            
        sline.clear();
        sline.str(line);

        // Read layer number
        getline(sline, value, ',');

        if (value > prevLayer)
        {
            prevLayer = value;
            if (layer.size() > 0)
                m_layers.push_back(layer);
            layer.clear();
        }

        // Read number of params in the signal Sj
        getline(sline, value, ',');

        VecDouble_t signal_j( std::stoi(value) );            
        for(int i = 0; i < signal_j.size(); ++i)
        {            
            getline(sline, value, ',') ;
            signal_j[i] = std::stod(value);                     
        }

        layer.push_back(signal_j);
    }       

    file.close();
    m_layers.push_back(layer);    
    // std::cout << "---------LOAD MODEL" << std::endl;
    // std::cout << *this << std::endl;
}

std::ostream & operator<<(std::ostream &os, const Net_t &net) {    
    for(std::size_t layer = 0; layer < net.m_layers.size(); ++layer) {
        os << "### begin Layer traspose " << std::to_string(layer) << " ###" << std::endl;

        for(std::size_t signal_j = 0; signal_j < net.m_layers[layer].size(); ++signal_j) {
            std::string signal_str{ std::to_string(signal_j) };            

            os << "(\t";
            auto* signal_ref = &net.m_layers[layer][signal_j];
            for(std::size_t weight = 0; weight < signal_ref->size(); ++weight) {                                
                os << (*signal_ref)[weight] << "\t";
            }
            os << ")" << std::endl;
        }

        os << "### end Layer " << std::to_string(layer) << " ###" << std::endl;;
    }

    return os;
}


// ######################## BEGIN AUXILIAR METHODS ########################
VecDouble_t Net_t::multiplyT(VecDouble_t const &x, MatDouble_t const &W) const
{
    VecDouble_t result(W.size(), 0.0);

    for (std::size_t i = 0; i < W.size(); ++i)
    {
        for (std::size_t k = 0; k < W[i].size(); ++k)
        {
            result[i] += x[k] * W[i][k];
        }
    }
    return result;
}

double Net_t::sigmoid(double x) const
{
    return 1.0 / (1.0 + std::exp(-x));
}

// sigmoid(value) * (1 -  sigmoid(value))
// x = sigmoid(value)
double Net_t::sigmoid_derivative(double x) const
{
    return x * (1 - x);
}

VecDouble_t Net_t::sigmoid(VecDouble_t const &v) const
{
    VecDouble_t result(v);

    for (auto &e : result)
        e = sigmoid(e);

    return result;
}

VecDouble_t Net_t::feedforward(VecDouble_t const &x) const
{
    VecDouble_t result(x);

    for (auto const &Wi : m_layers)
    {
        result.resize(result.size() + 1);
        std::copy(result.rbegin() + 1, result.rend(), result.rbegin());
        result[0] = 1.0;

        result = sigmoid( multiplyT(result, Wi) );
    }

    return result;
}

VecDouble_t Net_t::calculate_last_delta(VecDouble_t const &a, VecDouble_t const &y)
{
    VecDouble_t result(a.size(), 0.0);
    for (std::size_t i = 0; i < a.size(); ++i)
    {
        double deltax = 2 * (a[i] - y[i]) * sigmoid_derivative(a[i]);
        result[i] = deltax;
    }
    return result;
}

VecDouble_t Net_t::calculate_hidden_delta(VecDouble_t const &a, MatDouble_t const &last_layer, VecDouble_t const &deltas)
{
    VecDouble_t result(last_layer[0].size() - 1, 0.0);

    // iterate through previous layer neurons (by columns)
    for (size_t i{0}; i < last_layer[0].size() - 1; ++i)
    {
        // iterate over each weight of the neuron (same size as deltas)
        for (size_t j{0}; j < last_layer.size(); ++j)
        {
            result[i] += deltas[j] * last_layer[j][i + 1];
        }

        result[i] *= sigmoid_derivative(a[i]);
    }
    return result;
}

void Net_t::update_weights(MatDouble_t &layer, VecDouble_t const &a, VecDouble_t const &delta, double const &lr, double const &lambda)
{    
    for (size_t i{0}; i < layer.size(); ++i)
    {
        // update bias
        double grad = delta[i] * lr;
        layer[i][0] = layer[i][0] - grad;
        print("--Grad", i, 0, " \t\t", grad, "\n");

        // update weights        
        for (std::size_t j = 1; j < layer[0].size(); ++j)
        {
            double regularization = 2.0 * lambda * layer[i][j];
            grad = a[j - 1] * delta[i] + regularization;
            layer[i][j] = layer[i][j] - (lr * grad);
            print("--Grad", i, j, " \t\t", grad, "\n");
        }
    }
}

bool Net_t::checksize(int a, int b)
{
    if (a != b)
    {        
        throw std::runtime_error("Different size!!");
        return false;
    }

    return true;
}
// ######################## END AUXILIAR METHODS ########################