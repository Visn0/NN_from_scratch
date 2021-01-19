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
#include <sstream>
#include <utility>

using VecDouble_t       = std::vector<double>;      // = weight conection
using MatDouble_t       = std::vector<VecDouble_t>; // = layer
using VecPair_t         = std::vector<std::pair<double, double>>;
using PairMatDouble_t   = std::pair<MatDouble_t, MatDouble_t>;

#include "utils.h"

static const bool DEBUG = false;

template <class T>
static void print(T const &t)
{   
    if (!DEBUG) return;
    std::cout << " " << t;
}

static void print(VecDouble_t const &v)
{
    if (!DEBUG) return;

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
    if (!DEBUG) return;

    print(t);
    print(args...);
}

static void print(MatDouble_t const &mat)
{
    if (!DEBUG) return;

    std::cout << ">\n";
    for (auto const &vec : mat)
    {
        print(vec);
    }
    std::cout << ">\n";
}

static MatDouble_t CSV_to_MatDouble(const std::string& filename, const int& size) {
    std::ifstream file;
    file.open(filename);

    if(!file.is_open())
    {
        throw std::runtime_error("[EXCEPTION]: File not found : " + filename);
    }
    
    MatDouble_t data; 

    std::string line("");                
    std::istringstream sline;       
    
    while(getline(file, line))
    {                            
        sline.clear();
        sline.str(line);
              
        VecDouble_t input(size);    
        for(int i = 0; i < size; ++i)
        {
            sline >> std::hex >> input[i];            
        }

        data.push_back(input);
    }       
    
    file.close();

    // print(data);
    return data;
}

static void vecPair_to_CSV(const std::string &filename, const VecPair_t& vec) {
    std::ofstream file;
    file.open(filename);

    for(auto& v: vec) {
        file << std::to_string(v.first) << ";" << std::to_string(v.first) << ";" << std::endl;
    }

    file.close();
}

#endif