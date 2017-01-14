#pragma once

namespace docopt
{
    struct value;
}

namespace HonoursProject
{
    class Charset;
    class HashCracker;
}

class Program
{
public:

    Program(std::vector<std::string> arguments);

    int run();

private:

    static const char USAGE[];

    std::map<std::string, docopt::value> args;

    std::shared_ptr<HonoursProject::HashCracker> hash_cracker;

    void processUserCommand(char cmd);

};
