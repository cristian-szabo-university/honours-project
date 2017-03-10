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

class comma_num_punct : public std::numpunct<char>
{
protected:

    virtual char do_thousands_sep() const;

    virtual std::string do_grouping() const;

};

class Program
{
public:

    Program(std::vector<std::string> arguments);

    int run();

private:

    static const char USAGE[];

    std::map<std::string, docopt::value> args;

    std::string hash_msg;

    std::shared_ptr<HonoursProject::HashFactory> hash_factory;

    std::shared_ptr<HonoursProject::HashCracker> hash_cracker;

    std::locale comma_locale;

    void process_input_command(char cmd);

    std::string format_display_time(std::chrono::seconds tm);

    std::string format_display_speed(double speed_time, double exec_time);

};
