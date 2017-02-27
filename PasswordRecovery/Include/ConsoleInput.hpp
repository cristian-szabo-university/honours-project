#pragma once

#include "HashCracker.hpp"

class ConsoleInput : public HonoursProject::TTask<char>
{
public:

    ConsoleInput();

    virtual ~ConsoleInput();

    virtual char run() override;

};
