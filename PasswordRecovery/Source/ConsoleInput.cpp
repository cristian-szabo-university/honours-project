/**
    ConsoleInput.cpp

    Project Name: PasswordRecovery

    Copyright (c) 2017 University of the West of Scotland
    
    Authors: Ioan-Cristian Szabo <cristian.szabo@outlook.com>
*/

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
