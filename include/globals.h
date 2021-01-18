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

template <class T>
void print(T const &t);

void print(VecDouble_t const &v);

template <class T, class... Args>
void print(T const &t, Args... args);

void printVector(VecDouble_t const &vec);

void printMatrix(MatDouble_t const &mat);

#endif