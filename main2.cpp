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
                fillVectorRandom(v, -100.0, 100.0);
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
        return 1 / (1 + std::exp(-x));
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

    VecDouble_t calculate_delta(VecDouble_t const &a, VecDouble_t const &y)
    {
        VecDouble_t result(a.size(), 0.0);
        for (std::size_t i = 0; i < a.size(); ++i)
        {
            double sign = sigmoid(a[i]);
            double deltax = 2 * (a[i] - y[i]) * (sign * (1.0 - sign));
            result[i] = deltax;
        }
        return result;
    }

    VecDouble_t calculate_next_delta(VecDouble_t const &a, MatDouble_t const &last_layer, VecDouble_t const &delta)
    {
        VecDouble_t result(last_layer.size(), 0.0);
        for (size_t wi{0}; wi < last_layer.size(); ++wi)
        {
            double sign = sigmoid(a[wi]);
            double x = delta[wi] * (sign * (1.0 - sign));
            for (auto &wij : last_layer[wi])
            {
                result[wi] += wij * x;
            }
            result[wi] /= last_layer[wi].size();
        }
        return result;
    }

    void correct_weights(MatDouble_t &layer, VecDouble_t const &a, VecDouble_t const &delta, double const &lr)
    {
        for (size_t wi{0}; wi < layer.size(); ++wi)
        {
            double grad = a[wi] * delta[wi] * lr;
            std::cout << "--Grad " << wi << " \t\t" << grad << std::endl;
            for (int wij = 0; wij < layer[wi].size(); ++wij)
            {
                layer[wi][wij] = layer[wi][wij] - grad;
                //std::cout << layer[wi][wij] << std::endl;
            }
        }
    }

    void train(MatDouble_t const &X, VecDouble_t const &y, double const &lr)
    {
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
            for (size_t wi{0}; wi < m_layers.size(); ++wi)
            {
                result.resize(result.size() + 1);
                std::copy(result.rbegin() + 1, result.rend(), result.rbegin());
                result[0] = 1.0;

                VecDouble_t z = multiplyT(result, m_layers[wi]);
                result = sigmoid(z);
                outputs.push_back(std::make_pair(z, result));

                std::cout << "Output " << wi << "\t";
                printVector(result);
            }

            std::cout << "~~~~~~~~~~~~~~~\n";
            std::cout << "BACKPROPAGATION \n";
            std::cout << "~~~~~~~~~~~~~~~\n";
            int L = m_layers.size() - 1;
            VecDouble_t z = outputs[L].first;
            VecDouble_t a = outputs[L].second;

            VecDouble_t delta = calculate_delta(a, y);
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

            correct_weights(m_layers[L], a, delta, lr);
            //printMatrix(m_layers[L]);

            for (int l = L - 1; l > -1; --l)
            {
                z = outputs[l].first;
                a = outputs[l].second;
                delta = calculate_next_delta(a, last_layer, delta);
                std::cout << "Delta " << l << "\t\t";
                printVector(delta);

                last_layer = MatDouble_t(m_layers[l].size());
                for (std::size_t wi = 0; wi < m_layers[l].size(); ++wi)
                {
                    VecDouble_t w(m_layers[l][wi]);
                    last_layer[wi] = w;
                }
                correct_weights(m_layers[l], a, delta, lr);
            }
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
    //std::cout << evaluateNet(net, g_X, g_y) << std::endl;
    net.train(g_X, g_y, 0.1);
    std::cout << evaluateNet(net, g_X, g_y) << std::endl;
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