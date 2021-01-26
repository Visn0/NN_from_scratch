#include "utils.h"
#include "net_t.h"
#include <string.h>

const double REGULARIZATION_LAMBDA  = 0.001;
const uint32_t BATCH_SIZE           = 72;
const int EPOCHS                    = 200;
const double LEARNING_RATE          = 0.01;
const int OUTPUT_SIZE               = 3;
uint16_t INPUT_SIZE                 = 0;

PairMatDouble_t readDataset(const std::string& X_filename, const std::string& y_filename)
{
    MatDouble_t X ( CSV_to_MatDouble(X_filename, INPUT_SIZE) );
    MatDouble_t y ( CSV_to_MatDouble(y_filename, OUTPUT_SIZE) );         

    std::cout << "DATASET READ SUCCESFULLY" << std::endl;
    return std::make_pair(X, y);
}

// Time in ms
double net_fit(
      auto& net
    , const MatDouble_t& X_train
    , const MatDouble_t& y_train
    , const MatDouble_t& X_test
    , const MatDouble_t& y_test
) 
{
    double start = clock();

    VecPair_t history(
        net.fit(
              X_train
            , y_train
            , LEARNING_RATE
            , BATCH_SIZE
            , EPOCHS     
            , X_test
            , y_test
            , 1 // verbose,            
            , REGULARIZATION_LAMBDA
        )
    );

    double end = clock();

    vecPair_to_CSV("history.csv", history);    
    return (end - start) / CLOCKS_PER_SEC;
}


void run(int argc, char* argv[]) 
{   
    auto [X_train, y_train] = readDataset("X_train.csv", "y_train.csv");
    auto [X_valid, y_valid] = readDataset("X_valid.csv", "y_valid.csv"); 
    auto [X_test, y_test] = readDataset("X_test.csv", "y_test.csv");    

    INPUT_SIZE = X_train[0].size();
    std::cout << "INPUT_SIZE: " << INPUT_SIZE << std::endl;
    std::cout << "X_train SIZE: " << X_train.size() << std::endl;
    Net_t net{ INPUT_SIZE, 14, 7, OUTPUT_SIZE };
    
    const double fit_time = net_fit(net, X_train, y_train, X_valid, y_valid);

    for(size_t i{0}; i < 5; i++)
    {
        print("\n", X_test[i]);
        print(net.predict(X_test[i]));
        print(y_test[i], "\n");
    }

    std::cout << "Fit time: " << fit_time << " ms" << std::endl;
    
    net.save_model("ale_bot.csv");
}

int main(int argc, char* argv[])
{    
    try
    {
        run(argc, argv);
    }
    catch (std::exception &e)
    {
        std::cout << "[EXCEPTION]: " << e.what() << std::endl;
    }
    
    return 0;
}