/**
    Program.hpp

    Project Name: PasswordRecovery

    Copyright (c) 2017 University of the West of Scotland
    
    Authors: Ioan-Cristian Szabo <cristian.szabo@outlook.com>
*/

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

    std::string hash_msg;

    std::shared_ptr<HonoursProject::HashFactory> hash_factory;

    std::locale comma_locale;

    void process_input_command(std::shared_ptr<HonoursProject::CrackerTask> cracker_task, char cmd);

    std::string format_display_time(std::chrono::seconds tm, bool short_name = false);

    std::string format_display_speed(double speed_time, double exec_time);

    std::string format_benchmark_test(const std::string& name, double device_speed, std::size_t size, const std::string& start_mask, const std::string& append_mask);

};
