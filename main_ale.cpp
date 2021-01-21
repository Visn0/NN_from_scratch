#include "utils.h"
#include "net_t.h"

const int EPOCHS            = 100;
const double LEARNING_RATE  = 0.01;
const int INPUT_SIZE        = 128;
const int OUTPUT_SIZE       = 4;

void countClicks(const MatDouble_t& steps) {
    //
    // 0 - UP
    // 1 - SPACE
    // 2 - LEFT
    // 3 - RIGHT    
    //
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
    
    myshuffle(X);
    myshuffle(y);

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
            , EPOCHS     
            , X_test
            , y_test
            , 1 // verbose
        )
    );

    double end = clock();

    vecPair_to_CSV("history.csv", history);    
    return 1000.0 * (end - start) / CLOCKS_PER_SEC;
}

// Time in ms
double evaluate_time(auto& net, const MatDouble_t& X, const MatDouble_t& y)
{
    double start = clock();
    
    std::cout << "Evaluate error: " << net.evaluate(X, y) << std::endl;

    double end = clock();

    return 1000.0 * (end - start) / CLOCKS_PER_SEC;
}

void run() {
    Net_t net{ INPUT_SIZE, 64, 16, OUTPUT_SIZE };

    auto [X_train, y_train] = readDataset("x.csv", "y.csv");
    auto [X_test, y_test] = readDataset("x.csv", "y.csv");    

    countClicks(y_train);

    const double fit = fit_time(net, X_train, y_train, X_test, y_test);

    const double evaluate = evaluate_time(net, X_test, y_test);

    std::cout << "### EXECUTION TIMES ###" << std::endl;
    std::cout << "Fit\t\t: " << fit << " ms" << std::endl;
    std::cout << "Evaluate\t: " << evaluate << " ms" << std::endl;

    net.save_model("ale_bot.csv");
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