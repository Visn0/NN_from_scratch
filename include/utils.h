#ifndef _GLOBALS_H_
#define _GLOBALS_H_

#include <iostream>
#include <vector>
#include <cstdint>
#include <initializer_list>
#include <cassert>
#include <stdexcept>
#include <random>
#include <cmath>
#include <fstream>

using VecDouble_t = std::vector<double>;      // = weight conection
using MatDouble_t = std::vector<VecDouble_t>; // = layer

#include "utils.h"

template <class T>
static void print(T const &t)
{
    std::cout << " " << t;
}

static void print(VecDouble_t const &v)
{
    std::cout << "(";
    for (auto const &e : v)
    {
        std::cout << " " << e;
    }
    std::cout << " )\n";
}

template <class T, class... Args>
static void print(T const &t, Args... args) // recursive variadic function
{
    print(t);
    print(args...);
}

static void printVector(VecDouble_t const &vec)
{
    std::cout << "(";
    for (auto const &e : vec)
    {
        std::cout << " " << e;
    }
    std::cout << " )\n";
}

static void printMatrix(MatDouble_t const &mat)
{
    std::cout << ">\n";
    for (auto const &vec : mat)
    {
        printVector(vec);
    }
    std::cout << ">\n";
}

#endif