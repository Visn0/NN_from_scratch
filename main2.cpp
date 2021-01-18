#include <iostream>
#include <vector>
#include <cstdint>
#include <initializer_list>
#include <cassert>
#include <stdexcept>
#include <random>
#include <cmath>
#include <fstream>

// x (........)

/* 
    W [                 // layer inputs
        w0(........)    // weight 0
        w1(........)
        w2(........)
        w3(........)
        wn(........)
    ]
*/

using VecDouble_t = std::vector<double>;      // = weight conection
using MatDouble_t = std::vector<VecDouble_t>; // = layer

MatDouble_t g_X{{0.0, 0.0}, {0.0, 1.0}, {1.0, 0.0}, {1.0, 1.0}};
VecDouble_t g_y{0.0, 1.0, 1.0, 0.0};

MatDouble_t g_X2{{0.0, 0.0}, {0.0, 1.0}, {1.0, 0.0}, {1.0, 1.0}};
MatDouble_t g_y2{{0.0, 0.0}, {0.0, 1.0}, {0.0, 1.0}, {1.0, 0.0}};

void printVector(VecDouble_t const &vec)
{
    std::cout << "(";
    for (auto const &e : vec)
    {
        std::cout << " " << e;
    }
    std::cout << " )\n";
}

void printMatrix(MatDouble_t const &mat)
{
    std::cout << ">\n";
    for (auto const &vec : mat)
    {
        printVector(vec);
    }
    std::cout << ">\n";
}

double randDouble(double min, double max)
{
    static std::random_device dev;
    static std::mt19937 rng(0); //dev()); // random number generator
    static std::uniform_real_distribution<double> dist(min, max);
    return dist(rng);
}

void fillVectorRandom(VecDouble_t &vec, double min, double max)
{
    for (auto &v : vec)
    {
        v = randDouble(min, max);
    }
}

struct CutreNet_t
{
    explicit CutreNet_t(std::initializer_list<uint16_t> const &layers)
    {
        if (layers.size() < 2)
            throw std::out_of_range("CutreNet_t");

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

        // for (auto layer : m_layers)
        // {
        //     std::cout << layer.size() << std::endl;
        // }
    }

    VecDouble_t multiplyT(VecDouble_t const &x, MatDouble_t const &W) const
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
    double sigmoid(double x) const
    {
        return 1.0 / (1.0 + std::exp(-x));
    }

    VecDouble_t sigmoid(VecDouble_t const &v) const
    {
        VecDouble_t result(v);

        for (auto &e : result)
            e = sigmoid(e);

        return result;
    }

    VecDouble_t sigmoid_derivative(VecDouble_t const &v)
    {
        VecDouble_t result(v);

        for (auto &e : result)
        {
            e = sigmoid(e);
            e *= (1 - e);
        }
        return result;
    }

    VecDouble_t feedforward(VecDouble_t const &x) const
    {
        VecDouble_t result(x);

        for (auto const &Wi : m_layers)
        {
            result.resize(result.size() + 1);
            std::copy(result.rbegin() + 1, result.rend(), result.rbegin());
            result[0] = 1.0;

            result = sigmoid(multiplyT(result, Wi));
        }
        return result;
    }

    VecDouble_t calculate_delta(VecDouble_t const &a, double const &y)
    {
        VecDouble_t result(a.size(), 0.0);
        for (std::size_t i = 0; i < a.size(); ++i)
        {
            double sign = sigmoid(a[i]);
            double deltax = 2 * (sign - y) * (sign * (1.0f - sign));
            result[i] = deltax;
        }
        return result;
    }

    bool checksize(int a, int b)
    {
        if (a != b)
        {
            std::cout << "Different size!!" << std::endl;
            return false;
        }
        return true;
    }

    VecDouble_t calculate_next_delta(VecDouble_t const &a, MatDouble_t const &last_layer, VecDouble_t const &deltas)
    {
        VecDouble_t result(last_layer[0].size() - 1, 0.0);
        std::cout << "Inputs used: ";
        printVector(a);

        for (size_t i{0}; i < last_layer[0].size() - 1; ++i) // iterate through previous layer neurons (by columns)
        {
            double sign = sigmoid(a[i]);
            double derivsign = (sign * (1.0 - sign));
            for (size_t j{0}; j < last_layer.size(); ++j) // iterate over each weight of the neuron (same size as deltas)
            {
                result[i] += deltas[j] * last_layer[j][i + 1];
                std::cout << "Delta " << i << " = " << derivsign << " * " << deltas[j] << " * " << last_layer[j][i + 1] << "\n";
            }
            result[i] *= derivsign;
            //std::cout << "Delta i: \t" << result[i] << std::endl;
        }
        for (auto &layer : last_layer)
        {
            std::cout << "\n++Layer delta: ";
            printVector(layer);
        }
        return result;
    }

    void correct_weights(MatDouble_t &layer, VecDouble_t const &a, VecDouble_t const &delta, double const &lr)
    {
        for (size_t i{0}; i < layer.size(); ++i)
        {
            double grad = 1 * delta[i] * lr;
            layer[i][0] = layer[i][0] - grad;
            std::cout << "--Grad " << i << " " << 0 << " \t\t" << grad << std::endl;
            for (int j = 1; j < layer[0].size(); ++j)
            {
                double grad = a[j - 1] * delta[i] * lr;
                layer[i][j] = layer[i][j] - grad;
                std::cout << "--Grad " << i << " " << j << " \t\t" << grad << std::endl;
                //std::cout << layer[wi][wij] << std::endl;
            }
        }
    }

    void train(MatDouble_t const &X, VecDouble_t const &y, double const &lr)
    {
        std::ofstream myfile;
        myfile.open("example.csv");

        for (size_t epoch = 0; epoch < 100; ++epoch)
        {
            std::cout << "\n####################################################" << std::endl;
            std::cout << "EPOCH: " << epoch << std::endl;
            std::cout << "####################################################" << std::endl;
            for (size_t i{0}; i < X.size(); ++i) // iterations of train
            {
                std::vector<std::pair<VecDouble_t,  // raw output (z)
                                      VecDouble_t>> // output through activation function (a)
                    outputs;

                std::cout << "\n#################" << std::endl;
                std::cout << "Iteration " << i << std::endl;
                std::cout << "~~~~~~~~~~~~~~" << std::endl;

                std::cout << "NETWORK \n";
                for (size_t wi{0}; wi < m_layers.size(); ++wi)
                {
                    printMatrix(m_layers[wi]);
                }

                // feedforward
                std::cout << "~~~~~~~~~~~~~~~\n";
                std::cout << "FORWARD \n";
                std::cout << "~~~~~~~~~~~~~~~\n";
                VecDouble_t result(X[i]);
                std::cout << "Input \t";
                printVector(result);
                std::cout << "Expected \t" << y[i] << "\n";
                //printVector(y);
                for (size_t wi{0}; wi < m_layers.size(); ++wi)
                {
                    result.resize(result.size() + 1);
                    std::copy(result.rbegin() + 1, result.rend(), result.rbegin());
                    result[0] = 1.0;

                    VecDouble_t z = multiplyT(result, m_layers[wi]);
                    result = sigmoid(z);
                    outputs.push_back(std::make_pair(z, result));

                    std::cout << "Raw " << wi << "\t";
                    printVector(z);
                    std::cout << "Output " << wi << "\t";
                    printVector(result);
                }

                std::cout << "~~~~~~~~~~~~~~~\n";
                std::cout << "BACKPROPAGATION \n";
                std::cout << "~~~~~~~~~~~~~~~\n";
                int L = m_layers.size() - 1;
                VecDouble_t z = outputs[L].first;
                VecDouble_t a = outputs[L].second;

                VecDouble_t delta = calculate_delta(z, y[i]);
                std::cout << "Delta " << L << "\t\t";
                printVector(delta);

                // save last layer to use the original weights in next layer
                MatDouble_t last_layer(m_layers[L].size());
                for (std::size_t wi = 0; wi < m_layers[L].size(); ++wi)
                {
                    VecDouble_t w(m_layers[L][wi]);
                    last_layer[wi] = w;
                }
                //std::cout << "Last layer \n";
                //printMatrix(m_layers[L]);

                correct_weights(m_layers[L], outputs[L - 1].second, delta, lr);
                //printMatrix(m_layers[L]);

                for (int l = L - 1; l > -1; --l)
                {
                    z = outputs[l].first;
                    a = outputs[l].second;
                    delta = calculate_next_delta(z, last_layer, delta);
                    std::cout << "Delta " << l << "\t\t";
                    printVector(delta);

                    last_layer = MatDouble_t(m_layers[l].size());
                    for (std::size_t wi = 0; wi < m_layers[l].size(); ++wi)
                    {
                        VecDouble_t w(m_layers[l][wi]);
                        last_layer[wi] = w;
                    }
                    if (l == 0)
                    {
                        correct_weights(m_layers[l], X[i], delta, lr);
                    }
                    else
                    {
                        correct_weights(m_layers[l], outputs[l - 1].second, delta, lr);
                    }
                }

                double error = evaluateNet(X, y) / y.size();
                std::cout << error << std::endl;
                myfile << error << "\n";
            }
        }
        myfile.close();
    }

    double evaluateNet(MatDouble_t const &X, VecDouble_t const &y)
    {
        assert(X.size() == y.size());

        double error{0};
        for (std::size_t i{0}; i < y.size(); ++i)
        {
            auto h = feedforward(X[i]);
            double e = (h[0] - y[i]) * (h[0] - y[i]);
            //std::cout << "Error " << e << std::endl;
            error += e;
        }
        return error;
    }

private:
    std::vector<MatDouble_t> m_layers;
};

auto random_train(MatDouble_t const &X, VecDouble_t const &y, uint32_t maxiter)
{
    CutreNet_t bestNet{2, 3, 1};
    double bestError{bestNet.evaluateNet(X, y)};
    std::cout << "Iter 0 \t"
              << "Error: " << bestError << std::endl;

    for (uint32_t iter{0}; iter < maxiter; ++iter)
    {
        CutreNet_t newNet{2, 3, 1};
        double error{newNet.evaluateNet(X, y)};

        if (error < bestError)
        {
            bestNet = newNet;
            bestError = error;
            std::cout << "Iter " << iter << "\t Error: " << bestError << std::endl;
        }
    }

    return bestNet;
}

void run()
{
    // auto net = random_train(products, result, 10000);

    // for (std::size_t i{0}; i < products.size(); ++i)
    // {
    //     auto h = net.feedforward(products[i]);
    //     //printVector(products[i]);
    //     std::cout << h[0] << " " << result[i] << std::endl;
    // }

    CutreNet_t net{2, 3, 1}; // input_size, 1st layer_size, .. , output_layer_size
    //std::cout << evaluateNet(net, g_X, g_y) << std::endl;
    net.train(g_X, g_y, 0.01);
    std::cout << net.evaluateNet(g_X, g_y) / g_y.size() << std::endl;
}

int main()
{
    try
    {
        run();
    }
    catch (std::exception &e)
    {
        std::cout << "[EXCEPTION]: " << e.what() << std::endl;
    }
    return 0;
}