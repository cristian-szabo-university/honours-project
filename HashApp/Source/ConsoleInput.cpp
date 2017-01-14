#include "ConsoleInput.hpp"

ConsoleInput::ConsoleInput()
{
}

ConsoleInput::~ConsoleInput()
{
}

char ConsoleInput::run()
{
    char result;

    std::cout << "Command: ([s]tatus [p]ause [r]esume [q]uit) => ";
    std::cin >> result;

    return result;
}
