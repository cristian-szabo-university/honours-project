#include "Main.hpp"

#include "HashCracker.hpp"

#include "docopt.h"

#include "Program.hpp"

int main(int argc, char** argv)
{
    int error_code = 0;
    
    try
    {
        Program app({ argv + 1, argv + argc });

        error_code = app.run();
    }
    catch (std::exception ex)
    {
        std::cout << ex.what() << std::endl;

        error_code = 1;
    }

    return error_code;
}
