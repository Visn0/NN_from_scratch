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
    }

    VecDouble_t multiplyT(VecDouble_t const& x, MatDouble_t const& W) const
    {
        VecDouble_t result(W.size(), 0.0);

        for(std::size_t i = 0; i < W.size(); ++i)
        {
            for(std::size_t k=0; k < W[i].size(); ++k)
            {
                result[i] += x[k] * W[i][k];
            }
        }
        return result;
    }

    constexpr auto sigmoid(auto x) const
    {
        return 1 / (1 + std::exp(-x));
    }

    VecDouble_t sigmoid(VecDouble_t const& v) const
    {
        VecDouble_t result(v);

        for (auto& e: result) e = sigmoid(e);

        return result;
    }

    VecDouble_t feedforward(VecDouble_t const &x) const
    {
        VecDouble_t result(x);

        for (auto const &Wi : m_layers)
        {
            result.resize(result.size() + 1);
            std::copy(result.rbegin()+1, result.rend(), result.rbegin());
            result[0] = 1.0;

            result = sigmoid(multiplyT(result, Wi));
        }
        return result;
    }

private:
    std::vector<MatDouble_t> m_layers;
};

void run()
{
    CutreNet_t net{3, 4, 1}; // input_size, 1st layer_size, .. , output_layer_size

    auto v = net.feedforward({1.0, 0.0, 1.0});
    std::cout << v[0] << std::endl;
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