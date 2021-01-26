#pragma once

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
#include <algorithm>    // std::random_shuffle
#include <cstdlib>      // std::rand, std::srand

using VecInt_t              = std::vector<int>;
using VecDouble_t           = std::vector<double>;      // = weight conection
using MatDouble_t           = std::vector<VecDouble_t>; // = layer
using VecPair_t             = std::vector<std::pair<double, double>>;
using PairMatDouble_t       = std::pair<MatDouble_t, MatDouble_t>;
using VecPairVecDouble_t    = std::vector<std::pair<VecDouble_t, VecDouble_t>>;

static double randDouble(double min, double max)
{
    static std::random_device dev;
    static std::mt19937 rng(0); //dev()); // random number generator
    static std::uniform_real_distribution<double> dist(min, max);

    return dist(rng);
}

static void fillVectorRandom(VecDouble_t &vec, double min, double max)
{
    for (auto &v : vec)
    {
        v = randDouble(min, max);
    }
}

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

static void print(MatDouble_t const &mat)
{
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
        std::string value;

        if (size > 0) 
        {            
            for(int i = 0; i < size; ++i)
            {            
                getline(sline, value, ',') ;            
                input[i] = std::stod(value);
                // std::cout << sline.str() << std::endl;                      
            }
        }
        else
        {
            while(getline(sline, value, ','))
                input.push_back(std::stod(value));
        }

        data.push_back(input);
    }       
    
    file.close();
    return data;
}

static void vecPair_to_CSV(const std::string &filename, const VecPair_t& vec) {
    std::ofstream file;
    file.open(filename);

    for(auto& v: vec) {
        file << std::to_string(v.first) << "," << std::to_string(v.second) << std::endl;
    }

    file.close();
}

static void vecInt_to_txt(const std::string &filename, const VecInt_t& vec)
{
    std::ofstream file;
    file.open(filename);

    for(auto& v: vec) {
        file << v << std::endl;
    }

    file.close();
}

static void copySize(std::vector<MatDouble_t> &dest, std::vector<MatDouble_t> const &src)
{
    for (std::size_t i = 0; i < src.size(); ++i)
    {
        MatDouble_t mat(src[i].size());
        for (std::size_t j = 0; j < src[i].size(); ++j)
        {
            mat[j].resize(src[i][j].size());
        }
        dest.push_back(mat);            
    }
}

static void readRamIndexes(const std::string& filename, VecInt_t& indexes)
{
    std::ifstream file;
    file.open(filename);

    if(!file.is_open())
    {      
        throw std::runtime_error("[EXCEPTION]: File not found : " + filename);
    }
    std::string line("");           

    while(getline(file, line))
    {                            
        indexes.push_back( std::stoi(line) );
    }       

    file.close();   
}