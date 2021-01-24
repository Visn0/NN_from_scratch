#include "utils.h"
#include "net_t.h"

MatDouble_t X{{0.0, 0.0}, {0.0, 1.0}, {1.0, 0.0}, {1.0, 1.0}};
MatDouble_t y{{0.0}, {1.0}, {1.0}, {0.0}};

void run()
{
    Net_t net{2, 3, 1}; // input_size, 1st layer_size, .. , output_layer_size
    
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
    net.save_model("pruebasave.csv");
    net.printArchitecture();
}

int main()
{
    try
    {
        run();
        Net_t red("pruebasave.csv");
        std::cout << red << std::endl;
        red.printArchitecture();
    }
    catch (std::exception &e)
    {
        std::cout << "[EXCEPTION]: " << e.what() << std::endl;
    }
    return 0;
}