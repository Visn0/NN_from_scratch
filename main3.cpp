#include <iostream>
#include <vector>
#include <cstdint>
#include <initializer_list>
#include <cassert>
#include <stdexcept>
#include <random>
#include <cmath>

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

void printVector(VecDouble_t const &vec)
{
    std::cout << "(";
    for (auto const &e : vec)
    {
        std::cout << " " << e;
    }
    std::cout << ")\n";
}

double randDouble(double min, double max)
{
    static std::random_device dev;
    static std::mt19937 rng(dev()); // random number generator
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

    VecDouble_t subtract(VecDouble_t const &a, VecDouble_t const &b)
    {
        VecDouble_t result(a.size(), 0.0);

        for (std::size_t i = 0; i < a.size(); ++i)
        {
            result[i] = a[i] - b[i];
        }
        return result;
    }

    MatDouble_t subtract(MatDouble_t const &a, MatDouble_t const &b)
    {
        MatDouble_t result;

        for (std::size_t i = 0; i < a.size(); ++i)
        {
            result.push_back(subtract(a[i], b[i]));
        }
        return result;
    }

    VecDouble_t multiply(VecDouble_t const &a, VecDouble_t const &b)
    {
        VecDouble_t result(a.size(), 0.0);

        for (std::size_t i = 0; i < a.size(); ++i)
        {
            result[i] = a[i] * b[i];
        }
        return result;
    }

    MatDouble_t copy_layer(MatDouble_t const &from)
    {
        MatDouble_t result(from.size());
        for (std::size_t i = 0; i < result.size(); ++i)
        {
            result[i].resize(from[i].size());
            std::copy(from[i].begin(), from[i].end(), result.begin());
        }
        return result;
    }

    VecDouble_t scalar(VecDouble_t const &a, int const &s)
    {
        VecDouble_t result(a.size(), 0.0);

        for (std::size_t i = 0; i < a.size(); ++i)
        {
            result[i] = a[i] * s;
        }
        return result;
    }

    constexpr auto sigmoid(auto x) const
    {
        return 1 / (1 + std::exp(-x));
    }

    VecDouble_t sigmoid(VecDouble_t const &v) const
    {
        VecDouble_t result(v);

        for (auto &e : result)
            e = sigmoid(e);

        return result;
    }

    VecDouble_t sigmoid_derivative(VecDouble_t const &v) const
    {
        VecDouble_t result(v);

        for (auto &e : result)
        {
            e = sigmoid(e);
            e *= (1 - e);
        }
        return result;
    }

    MatDouble_t gradient(VecDouble_t const &a, VecDouble_t const &deltas, double const &lr)
    {
        MatDouble_t result;
        for (auto delta : deltas)
        {
            result.push_back(scalar(a, delta * lr));
        }
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

    void train(MatDouble_t const &X, VecDouble_t const &y, double const &lr)
    {
        for (size_t i{0}; i < X.size(); ++i) // iterations of train
        {
            std::vector<std::pair<VecDouble_t,  // raw output (z)
                                  VecDouble_t>> // output through activation function (a)
                outputs;

            std::cout << "#################" << std::endl;
            std::cout << "Iteration " << i << std::endl;
            std::cout << "#################" << std::endl;

            // feedforward
            VecDouble_t result(X[i]);
            for (auto const &Wi : m_layers)
            {
                result.resize(result.size() + 1);
                std::copy(result.rbegin() + 1, result.rend(), result.rbegin());
                result[0] = 1.0;

                VecDouble_t z = multiplyT(result, Wi);
                result = sigmoid(z);
                outputs.push_back(std::make_pair(z, result));
            }

            std::vector<VecDouble_t> deltas;
            int L = m_layers.size() - 1;
            VecDouble_t z = outputs[L].first;
            VecDouble_t a = outputs[L].second;

            // deltas.push_back(multiply(scalar(subtract(a, y), 2), sigmoid_derivative(a)));
            // printVector(deltas[0]);

            //MatDouble_t grad = gradient(a, deltas[0], lr);

            //MatDouble_t last_layer;
            // for (std::size_t i = 0; i < last_layer.size(); ++i)
            // {
            //     last_layer[i].resize(m_layers[L][i].size());
            //     //std::copy(m_layers[L][i].begin(), m_layers[L][i].end(), last_layer.begin());
            //     //for (std::size_t i = 0; i < last_layer.size(); ++i)
            // }
            //MatDouble_t last_layer = copy_layer(m_layers[L]); // to use the original weights in next layer

            // m_layers[L] = subtract(m_layers[L], grad);

            // for (size_t l = L - 1; l > -1; --l)
            // {
            //     z = outputs[l].first;
            //     a = outputs[l].second;
            //     deltas[0] = (multiply(multiplyT(deltas[0], last_layer), sigmoid_derivative(a)));
            //     last_layer = copy_layer(m_layers[l]);

            //     grad = gradient(a, deltas[0], lr);
            //     m_layers[l] = subtract(m_layers[l], grad);
            // }

            // for (auto &out : outputs)
            // {
            //     printVector(out.first);
            //     printVector(out.second);
            //     std::cout << "--------------------" << std::endl;
            // }
        }
    }

private:
    std::vector<MatDouble_t> m_layers;
};

MatDouble_t g_X{{0.0, 0.0}, {0.0, 1.0}, {1.0, 0.0}, {1.0, 1.0}};
VecDouble_t g_y{0.0, 1.0, 1.0, 0.0};

double evaluateNet(CutreNet_t const &net, MatDouble_t const &X, VecDouble_t const &y)
{
    assert(X.size() == y.size());

    double error{0};
    for (std::size_t i{0}; i < y.size(); ++i)
    {
        auto h = net.feedforward(X[i]);
        error += (h[0] - y[i]) * (h[0] - y[i]);
    }
    return error;
}

auto random_train(MatDouble_t const &X, VecDouble_t const &y, uint32_t maxiter)
{
    CutreNet_t bestNet{2, 3, 1};
    double bestError{evaluateNet(bestNet, X, y)};
    std::cout << "Iter 0 \t"
              << "Error: " << bestError << std::endl;

    for (uint32_t iter{0}; iter < maxiter; ++iter)
    {
        CutreNet_t newNet{2, 3, 1};
        double error{evaluateNet(newNet, X, y)};

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
    // auto net = random_train(g_X, g_y, 10000);

    // for (std::size_t i{0}; i < g_X.size(); ++i)
    // {
    //     auto h = net.feedforward(g_X[i]);
    //     printVector(g_X[i]);
    //     std::cout << h[0] << " " << g_y[i] << std::endl;
    // }

    CutreNet_t net{2, 3, 1}; // input_size, 1st layer_size, .. , output_layer_size
    net.train(g_X, g_y, 0.1);
}

int main()
{
    // try
    // {
    //     run();
    // }
    // catch (std::exception &e)
    // {
    //     std::cout << "[EXCEPTION]: " << e.what() << std::endl;
    // }
    // return 0;
}