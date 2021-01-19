#include "utils.h"
#include "net_t.h"

const int EPOCHS            = 1000;
const double LEARNING_RATE  = 0.01;
const int TRAIN_INPUT_SIZE  = 40;
const int TEST_INPUT_SIZE   = 40;

PairMatDouble_t readDataset(const std::string& X_filename, const std::string& y_filename, const int& input_size)
{
    MatDouble_t X ( CSV_to_MatDouble(X_filename, input_size) );
    MatDouble_t y ( CSV_to_MatDouble(y_filename, input_size) );

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

    net.fit(
          X_train
        , y_train
        , LEARNING_RATE
        , EPOCHS     
        , X_test
        , y_test
    );

    double end = clock();

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
    Net_t net{2, 100, 100, 1};

    auto [X_train, y_train] = readDataset("train.csv", "y_train.csv", TRAIN_INPUT_SIZE);
    auto [X_test, y_test] = readDataset("X_test.csv", "y_test.csv", TEST_INPUT_SIZE);

    const double fit = fit_time(net, X_train, y_train, X_test, y_test);

    const double evaluate = evaluate_time(net, X_test, y_test);

    std::cout << "### EXECUTION TIMES ###" << std::endl;
    std::cout << "Fit\t\t: " << fit << " ms" << std::endl;
    std::cout << "Evaluate\t: " << evaluate << " ms" << std::endl;
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