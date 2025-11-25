#include "Bus.hpp"

int main(int argc, char** argv)
{
    try
    {
        Bus bus;
    }
    catch (std::runtime_error e)
    {
        std::cerr << e.what() << std::endl;
    }
    catch (...)
    {
        std::cerr << "Unknown exception" << std::endl;
    }
    return 0;
}
