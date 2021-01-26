#include "utils.h"
#include "net_t.h"

MatDouble_t X{{0.0, 0.0}, {0.0, 1.0}, {1.0, 0.0}, {1.0, 1.0}};
MatDouble_t y{{0.0, 0.0}, {0.0, 1.0}, {0.0, 1.0}, {1.0, 0.0}};

void printPred(auto const &vec)
{
    std::cout << "( ";     
    for(std::size_t i = 0; i < vec.size(); ++i)
    {
        std::cout << vec[i] << " ";
    }
    std::cout << ")" << std::endl;
}

void run()
{
    Net_t net{2, 3, 2}; // input_size, 1st layer_size, .. , output_layer_size
    
    net.fit(
          X     // train dataset 
        , y     // train labels
        , 0.01  // learning rate
        , 1     // batch size
        , 1     // epochs
        , X
        , y
    );
    
    VecDouble_t prediction = net.predict(X[1]);
    printPred(prediction);
    net.save_model("main7_model.csv");

    Net_t net2("main7_model.csv");
    prediction = net2.predict(X[1]);
    printPred(prediction);
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