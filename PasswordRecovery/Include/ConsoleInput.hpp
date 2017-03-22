/**
    ConsoleInput.hpp

    Project Name: PasswordRecovery

    Copyright (c) 2017 University of the West of Scotland
    
    Authors: Ioan-Cristian Szabo <cristian.szabo@outlook.com>
*/

#pragma once

#include "HashCracker.hpp"

class ConsoleInput : public HonoursProject::TTask<char>
{
public:

    ConsoleInput();

    virtual ~ConsoleInput();

    virtual char run() override;

};
