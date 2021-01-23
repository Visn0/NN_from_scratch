#include "utils.h"
#include "net_t.h"

MatDouble_t X{{0.0, 0.0}, {0.0, 1.0}, {1.0, 0.0}, {1.0, 1.0}};
MatDouble_t y{{0.0, 0.0}, {0.0, 1.0}, {0.0, 1.0}, {1.0, 0.0}};

void run()
{
    Net_t net{2, 3, 2}; // input_size, 1st layer_size, .. , output_layer_size
    
    net.fit(
          X     // train dataset 
        , y     // train labels
        , 0.1   // learning rate
        , 1     // batch size
        , 1     // epochs
        , X
        , y
    );
    
    std::cout << std::endl << net << std::endl;
    std::cout << "Evaluate error: " << net.evaluate(X, y) << std::endl;
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