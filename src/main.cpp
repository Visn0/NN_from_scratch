#include "globals.h"
#include "net_t.h"

MatDouble_t X{{0.0, 0.0}, {0.0, 1.0}, {1.0, 0.0}, {1.0, 1.0}};
MatDouble_t y{{0.0}, {1.0}, {1.0}, {0.0}};

void run()
{
    Net_t net{2, 3, 2}; // input_size, 1st layer_size, .. , output_layer_size
    net.fit(X, y, 0.01);
    std::cout << net.evaluate(X, y) / y.size() << std::endl;
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