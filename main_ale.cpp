#include "utils.h"
#include "net_t.h"
#include <string.h>

const double REGULARIZATION_LAMBDA  = 0.0001;
const uint32_t BATCH_SIZE           = 64;
const int EPOCHS                    = 100;
const double LEARNING_RATE          = 0.1;
const int OUTPUT_SIZE               = 5;
uint16_t INPUT_SIZE                 = 0;

void heatMap(const MatDouble_t& data, const double& percentage) 
{
    // Initialize heat map
    VecDouble_t heat(data[0].size(), 0.0);
        
    // Calculate % of times that a memory address changes
    for(const auto& v: data)
    {
        for(std::size_t i = 0; i < heat.size(); ++i)
        {
            if (v[i] != data[0][i]) // ExampleData[i] != initialData[i]
            {
                heat[i] += (100.0/data.size());
            }
        }
    }

    uint8_t add = 0;
    std::printf("\n===================== HEAT MAP =====================");
    std::printf("\nAD | 000 001 002 003 004 005 006 007 008 009 00A 00B 00C 00D 00E 00F");
    std::printf("\n====================================================");
    for (std::size_t i = 0; i < 8; i++)
    {
        std::printf("\n%02X | ", add);
        for (std::size_t j = 0; j < 16; j++, add++)
        {
            std::printf("%03.0f ", heat[add]);
        }
    }
    std::printf("\n====================================================\n");

    VecInt_t indexes;
    std::cout << "Changing indexes: ( ";        
    for(std::size_t i = 0; i < heat.size(); ++i)
    {
        if (heat[i] > percentage)
        {            
            indexes.push_back(i);
            std::cout << i << " ";
        }
    }
    std::cout << ")" << std::endl;
    std::cout << std::endl << "TOTAL: " << indexes.size() << std::endl;

    vecInt_to_txt("ramindexes.txt", indexes);
}

void countClicks(const MatDouble_t& steps) {
    //
    // 0 - UP
    // 1 - SPACE
    // 2 - LEFT
    // 3 - RIGHT    
    //
    if (!DEBUG) return;

    VecDouble_t counters(4);
    int clickCounter = 0;
    for(const auto& step: steps) {
        int clicksOnStep = 0;

        for(std::size_t key = 0; key < step.size(); ++key) {
            counters[key] += step[key]; // step[key] is always 0 or 1.
            clicksOnStep += step[key];
        }      

        clickCounter += ((clicksOnStep > 0) ? 1 : 0);        
    }

    print("\n## begin CLICK COUNTERS\n");
    print("TOTAL examples: " + std::to_string(steps.size()) + "\n");
    print("Examples with click: " + std::to_string(clickCounter) + "\n");
    print("(UP, SPACE, LEFT, RIGHT)\n");
    print(counters);
    print("## end CLICK COUNTERS \n");
}

PairMatDouble_t readDataset(const std::string& X_filename, const std::string& y_filename)
{
    MatDouble_t X ( CSV_to_MatDouble(X_filename, INPUT_SIZE) );
    MatDouble_t y ( CSV_to_MatDouble(y_filename, OUTPUT_SIZE) );         

    std::cout << "DATASET READ SUCCESFULLY" << std::endl;
    return std::make_pair(X, y);
}

// Time in ms
double fit_time(
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

// Time in ms
double evaluate_time(auto& net, const MatDouble_t& X, const MatDouble_t& y)
{
    double start = clock();
    
    std::cout << "Evaluate error: " << net.evaluate(X, y) << std::endl;

    double end = clock();

    return 1000.0 * (end - start) / CLOCKS_PER_SEC;
}

void run(int argc, char* argv[]) 
{   
    auto [X_train, y_train] = readDataset("X_train.csv", "y_train.csv");
    auto [X_valid, y_valid] = readDataset("X_valid.csv", "y_valid.csv"); 
    auto [X_test, y_test] = readDataset("X_test.csv", "y_test.csv");    

    INPUT_SIZE = X_train[0].size();
    std::cout << "INPUT_SIZE: " << INPUT_SIZE << std::endl;
    Net_t net{ INPUT_SIZE, 48, 24, OUTPUT_SIZE };
    
    const double fit = fit_time(net, X_train, y_train, X_valid, y_valid);

    const double evaluate = evaluate_time(net, X_test, y_test);

    std::cout << "### EXECUTION TIMES ###" << std::endl;
    std::cout << "Fit\t\t: " << fit << " s" << std::endl;
    std::cout << "Evaluate\t: " << evaluate << " ms" << std::endl;

    net.save_model("ale_bot.csv");
}

int main(int argc, char* argv[])
{    
    try
    {
        if (argc > 2 && strcmp(argv[1], "-heat") == 0)
        {
            auto [X_train, y_train] = readDataset("X_train.csv", "y_train.csv");
            heatMap(X_train, atof(argv[2]));                
        } 
        else
        {
            run(argc, argv);
        }
    }
    catch (std::exception &e)
    {
        std::cout << "[EXCEPTION]: " << e.what() << std::endl;
    }
    
    return 0;
}