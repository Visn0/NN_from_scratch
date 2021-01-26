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

const double THRESHOLD = 0.5;

Net_t::Net_t(std::initializer_list<uint16_t> const &layers)
{
    if (layers.size() < 2)
        throw std::out_of_range("[EXCEPTION]: The number of layers must be at least 2.");

    this->input_size = *layers.begin();

    auto inp_size = *layers.begin();
    for (auto it = layers.begin() + 1; it != layers.end(); ++it)
    {
        if (inp_size == 0)
            throw std::out_of_range("[EXCEPTION]: The number neurons per layers must be > 0.");

        MatDouble_t layer_w(*it);
        for (auto &v : layer_w)
        {
            v.resize(inp_size + 1); // +threshold
            fillVectorRandom(v, -10.0, 10.0);
        }
        m_layers.push_back(layer_w);
        inp_size = *it;
    }
}

Net_t::Net_t(std::string const &filename) {
    load_model(filename);
}

Net_t::Net_t(std::uint16_t const &input_size, const std::vector<MatDouble_t>& weights)
{
    this->input_size = input_size;
    this->m_layers = weights;
}

VecPair_t Net_t::fit(
      MatDouble_t const &X_train
    , MatDouble_t const &y_train
    , double const &lr
    , uint32_t const& batch_size
    , std::size_t const &epochs
    , MatDouble_t const &X_validation
    , MatDouble_t const &y_validation
    , uint8_t const &verbose
    , double const &regularization_lambda
)
{
    checkDatasetSize(X_train, y_train, "train");
    checkDatasetSize(X_validation, y_validation, "validation");
            
    // Each pair contains <error_train, error_validation> for one epoch.
    // This vector will contain all this pairs sorted by the epoch number from lower to higher.
    VecPair_t history(epochs);    
    
    const std::size_t maxBatches = X_train.size() / batch_size + (int)(X_train.size() % batch_size != 0);    

    for (size_t epoch = 0; epoch < epochs; ++epoch)
    {                                    
        for(std::size_t batch = 0; batch < maxBatches; ++batch)
        {        
            std::vector<MatDouble_t> gradients; 
            copySize(gradients, m_layers);    

            std::vector<VecDouble_t> outputs; // output through activation function (a)

            std::size_t maxExample = batch * batch_size + batch_size;
            for (std::size_t i = batch * batch_size; i < X_train.size() && i < maxExample; ++i) // iterations of train in 1 batch
            {                                
                // Sets the outputs of each neuron of each layer in the outputs variable
                feedforward_train(outputs, X_train[i]);

                // Sets acumulations of the gradients for all weight in the Neural Network for the given example Xi, yi
                backpropagation_without_update_weights(gradients, outputs, X_train[i], y_train[i], regularization_lambda);                                
            }

            // If X_train.size() % batch_size != 0, the last batch, instead of using the batch_size, it uses the amount
            // of examples taken for the given batch. For example, suppose X_train.size()=100, batch_size=40
            // The first batch it takes batch_size, since there were enough examples for this batch (40;
            // The seconds batch used also 40, but the third batch has only 20 examples left, so instead of using again 40,
            // it uses 20, which is the amount of examples used in the feedforward. 
            const size_t current_batch_size = maxExample - (batch * batch_size);
            update_weights(gradients, lr, current_batch_size);          
        }
    
        const double train_error = evaluate(X_train, y_train);
        const double test_error  = evaluate(X_validation, y_validation);
        history[epoch] = std::make_pair( train_error, test_error ); 

        switch (verbose)
        {
            case 1:
                std::cout << "Epoch " << std::to_string(epoch);
                std::cout << ": train_error=" << std::to_string(train_error);
                std::cout << "\tval_error=" << std::to_string(test_error);
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

    error /= (y.size());
    return error;
}

VecDouble_t Net_t::predict(VecDouble_t const &X) const 
{    
    if (X.size() != (std::size_t) this->input_size)
    {
        throw std::out_of_range(
            "[EXCEPTION]: Invalid input data size. Expected size="
            + std::to_string(this->input_size)
            + ", received="
            + std::to_string(X.size())
        );
    }    

    return feedforward(X);
}

void Net_t::save_model(std::string const &filename) const{
    std::ofstream file;
    file.open(filename, std::ios::out);

    // First line is net architecture
    file << this->input_size;
    for(std::size_t layer = 0; layer < m_layers.size(); ++layer) {
        file << "," << m_layers[layer].size();
    }
    file << std::endl;

    // Weights of the network ordered by layers orderes by signals
    for(std::size_t layer = 0; layer < m_layers.size(); ++layer) {        
        for(std::size_t signal_j = 0; signal_j < m_layers[layer].size(); ++signal_j) {
            std::string signal_str{ std::to_string(signal_j) };            

            auto* signal_ref = &m_layers[layer][signal_j];

            file << layer; // layer number
            file << "," <<  signal_ref->size(); // Number of parameters in the signal Sj
            
            for(std::size_t weight = 0; weight < signal_ref->size(); ++weight) {                                                
                file << "," << (*signal_ref)[weight]; // weigth ij
            }

            file << std::endl;
        }        
    }

    file.close();
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
    
    // Line with architecture
    getline(file, line);
    sline.clear();
    sline.str(line);
    getline(sline, value, ',');

    this->input_size = std::stod(value);

    // Layers, signals and weights
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
        for(std::size_t i = 0; i < signal_j.size(); ++i)
        {            
            getline(sline, value, ',') ;
            signal_j[i] = std::stod(value);                     
        }

        layer.push_back(signal_j);
    }       

    file.close();
    m_layers.push_back(layer);    
}

void Net_t::printArchitecture() const
{    
    std::cout << "Architecture: { " << this->input_size;
    
    for(std::size_t layer = 0; layer < m_layers.size(); ++layer) {
        std::cout << " " << m_layers[layer].size();
    }

    std::cout << " }" << std::endl;
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

void Net_t::backpropagation_without_update_weights(
      std::vector<MatDouble_t>& gradients
    , std::vector<VecDouble_t> const &outputs
    , VecDouble_t const &Xi
    , VecDouble_t const &yi
    , double const &regularization_lambda
)
{
    // print("\n~~~~~~~~~~~~~~\n");
    // print("BACKPROPAGATION \n");
    // print("~~~~~~~~~~~~~~\n");
    const int L = m_layers.size() - 1;

    VecDouble_t last_delta = calculate_last_delta(outputs[L], yi);
    //print("Delta", L, "\t\t", last_delta); 
    calculate_gradients(gradients[L], m_layers[L], outputs[L-1], last_delta, regularization_lambda);
    
    for (int l = L - 1; l > 0; --l)
    {
        last_delta = calculate_hidden_delta(outputs[l], m_layers[l+1], last_delta);
        //print("Delta", l, "\t\t", last_delta);                    
        calculate_gradients(gradients[l], m_layers[l], outputs[l-1], last_delta, regularization_lambda);                    
    }  

    // Update weights related to the input layer, whose activation coeficients Xi are the input values.
    last_delta = calculate_hidden_delta(outputs[0], m_layers[1], last_delta);
    // print("Delta", 0, "\t\t", last_delta);
    calculate_gradients(gradients[0], m_layers[0], Xi, last_delta, regularization_lambda);
}

void Net_t::feedforward_train(std::vector<VecDouble_t>& outputs, VecDouble_t const &Xi)
{    
    VecDouble_t result(Xi);

    for (size_t wi{0}; wi < m_layers.size(); ++wi)
    {
        result.resize(result.size() + 1);
        std::copy(result.rbegin() + 1, result.rend(), result.rbegin());
        result[0] = 1.0;

        VecDouble_t z = multiplyT(result, m_layers[wi]);
        result = sigmoid(z);
        outputs.push_back(result);
    }
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
        int res = 0;
        if(a[i] > THRESHOLD)
        {
            res = 1;
        }
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

void Net_t::calculate_gradients(MatDouble_t &gradients, MatDouble_t &layer, VecDouble_t const &a, VecDouble_t const &delta, double const &lambda)
{    
    for (size_t i{0}; i < layer.size(); ++i)
    {        
        // update bias       
        double regularization = 2.0 * lambda * layer[i][0]; 
        gradients[i][0] += delta[i] + regularization;
        //print("--Grad", i, 0, " \t\t", gradients[i][0], "\n");

        // update weights        
        for (std::size_t j = 1; j < layer[0].size(); ++j)
        {
            regularization = 2.0 * lambda * layer[i][j];
            double grad = a[j - 1] * delta[i] + regularization;
            // layer[i][j] = layer[i][j] - (lr * grad);
            gradients[i][j] += grad;
            //print("--Grad", i, j, " \t\t", grad, "\n");
        }
    }
}

void Net_t::update_weights(std::vector<MatDouble_t> const &gradients, double const &lr, uint32_t const& batch_size)
{    
    // Select layer
    for(std::size_t l = 0; l < m_layers.size(); ++l)
    {
        // Select weights of signal Sj (row)
        for (size_t s = 0; s < m_layers[l].size(); ++s)
        {       
            // Select weigth of signal (column)
            for (std::size_t w = 0; w < m_layers[l][s].size(); ++w)
            {                
                // wij = wij - lr*gradient
                m_layers[l][s][w] = m_layers[l][s][w] - (lr * gradients[l][s][w]/batch_size);
            }
        }
    }
}

void Net_t::checkDatasetSize(MatDouble_t const &X, MatDouble_t const &y, std::string const &tag) const
{
    for (const auto& xi : X)
    {
        if (xi.size() != (std::size_t) this->input_size)
        {
            throw std::out_of_range(
                "[EXCEPTION]: Invalid input " + tag + " data size. Expected size="
                + std::to_string(this->input_size)
                + ", received="
                + std::to_string(xi.size())
            );
        }
    }
        
    // Size of the output layer
    static const std::size_t output_size = m_layers[ m_layers.size() - 1 ].size() ;

    for (const auto& yi : y)
    {
        if (yi.size() != output_size)
        {
            throw std::out_of_range(
                "[EXCEPTION]: Invalid label " + tag + " data size. Expected size="
                + std::to_string(output_size)
                + ", received="
                + std::to_string(yi.size())
            );
        }
    }
}
// ######################## END AUXILIAR METHODS ########################