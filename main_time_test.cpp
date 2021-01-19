#include "utils.h"
#include "net_t.h"

MatDouble_t X{{0.0, 0.0}, {0.0, 1.0}, {1.0, 0.0}, {1.0, 1.0}};
MatDouble_t y{{0.0}, {1.0}, {1.0}, {0.0}};

// Time in ms
double fit_time(auto& net) 
{
    double start = clock();

    net.fit(
          X     // train dataset 
        , y     // train labels
        , 0.01  // learning rate
        , 100     // epochs
        , X
        , y
    );

    double end = clock();

    return 1000.0 * (end - start) / CLOCKS_PER_SEC;
}

// Time in ms
double evaluate_time(auto& net)
{
    double start = clock();
    
    std::cout << "Evaluate error: " << net.evaluate(X, y) << std::endl;

    double end = clock();

    return 1000.0 * (end - start) / CLOCKS_PER_SEC;
}

int main()
{
    try
    {
        Net_t net{2, 100, 100, 1};

        const double fit = fit_time(net);
        const double evaluate = evaluate_time(net);

        std::cout << "### EXECUTION TIMES ###" << std::endl;
        std::cout << "Fit\t\t: " << fit << " ms" << std::endl;
        std::cout << "Evaluate\t: " << evaluate << " ms" << std::endl;
    }
    catch (std::exception &e)
    {
        std::cout << "[EXCEPTION]: " << e.what() << std::endl;
    }
    
    return 0;
}