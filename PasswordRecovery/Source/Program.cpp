#include "HashCracker.hpp"

#include "docopt.h"

#include "Program.hpp"

#include "ConsoleInput.hpp"

const char Program::USAGE[] =
R"(
Usage:
    HashCracker attack <hash> (bruteforce|dictionary) (md5|sha1) <input> [--device=<type>]
    HashCracker benchmark (bruteforce|dictionary) (md5|sha1) [--device=<type>]
    HashCracker -h | --help
    HashCracker --version

Options:
    -h --help     Show this screen.
    --version     Show version.
)";

Program::Program(std::vector<std::string> cli)
{
    args = docopt::docopt(USAGE, cli, true, "HashCracker 1.0");

    comma_locale = std::locale(std::locale(), new HonoursProject::Platform::comma_num_punct());
}

int Program::run()
{
    std::future<char> console_future;
    std::future<std::string> cracked_future; 
    std::shared_ptr<ConsoleInput> console_task = std::make_shared<ConsoleInput>();

    bool benchmark = args["benchmark"].asBool();

    if (args["md5"].asBool())
    {
        hash_factory = std::make_shared<HonoursProject::MD5>();
    }
    else if (args["sha1"].asBool())
    {
        throw std::runtime_error("Hash function not implemented yet!");
    }

    std::shared_ptr<HonoursProject::AttackFactory> attack_factory;

    if (args["bruteforce"].asBool())
    {
        attack_factory = std::make_shared<HonoursProject::Bruteforce>(hash_factory);
    }
    else if (args["dictionary"].asBool())
    {
        throw std::runtime_error("Attack not implemented yet!");
    }

    std::vector<std::string> attack_input;

    if (args["attack"].asBool())
    {
        attack_input.push_back(args["<hash>"].asString());

        if (args["bruteforce"].asBool())
        {
            attack_input.push_back(args["<input>"].asString());
        }
        else if (args["dictionary"].asBool())
        {
            throw std::runtime_error("Attack not implemented yet!");
        }
    }
    else if (benchmark)
    {
        attack_input.push_back(std::string(32, 'f'));

        if (args["bruteforce"].asBool())
        {
            attack_input.push_back("?a?a?a?a?a?a?a?a");
        }
        else if (args["dictionary"].asBool())
        {
            throw std::runtime_error("Attack not implemented yet!");
        }
    }

    hash_msg = attack_input[0];

    HonoursProject::HashCracker::DeviceFilter device_filter;

    if (args["--device"].isString())
    {
        std::string type = args["--device"].asString();

        if (type == "cpu")
        {
            device_filter = HonoursProject::HashCracker::DeviceFilter::CPU_ONLY;
        }
        else if (type == "gpu")
        {
            device_filter = HonoursProject::HashCracker::DeviceFilter::GPU_ONLY;
        }
        else
        {
            throw std::runtime_error("Invalid device type!");
        }
    }
    else
    {
        device_filter = HonoursProject::HashCracker::DeviceFilter::CPU_GPU;
    }
    
    HonoursProject::Logger::info("...:::=== Preparing Cracking Process ===:::...\n\n");

    std::shared_ptr<HonoursProject::HashCracker> hash_cracker = std::make_shared<HonoursProject::HashCracker>();

    if (!hash_cracker->create(device_filter))
    {
        return 2;
    }
    
    std::shared_ptr<HonoursProject::CrackerTask> cracker_task = hash_cracker->createCrackerTask(attack_input, attack_factory, benchmark);

    if (!cracker_task)
    {
        return 1;
    }

    cracked_future = std::async(std::launch::async, &HonoursProject::CrackerTask::run, cracker_task.get());

    if (benchmark)
    {
        std::this_thread::sleep_for(std::chrono::seconds(5));
        cracker_task->setStatus(HonoursProject::CrackerTask::Status::Aborted);

        cracked_future.get();

        HonoursProject::Logger::info("...:::=== Benchmark Statistics ===:::...\n\n");

        std::cout << std::setfill('.') << std::setw(25) << std::left << "Hash.Type";
        std::cout << ": " << hash_factory->type() << std::endl;

        double total_device_speed = 0.0, total_device_exec = 0.0;

        for (std::size_t device_pos = 0; device_pos < cracker_task->getDeviceNum(); device_pos++)
        {
            std::shared_ptr<HonoursProject::Device> device = cracker_task->getDeviceAt(device_pos);
            double device_speed = cracker_task->getDeviceSpeed(device_pos);
            double device_exec = cracker_task->getDeviceExec(device_pos);

            std::cout << std::setfill('.') << std::setw(25) << std::left << ("Speed.Device.#" + std::to_string(device->getId()));
            std::cout << ": " << format_display_speed(device_speed, device_exec) << std::endl;

            total_device_speed += device_speed;
            total_device_exec += device_exec;
        }

        if (cracker_task->getDeviceNum() > 1)
        {
            std::cout << std::setfill('.') << std::setw(25) << std::left << "Speed.Device.Total";
            std::cout << ": " << format_display_speed(total_device_speed, total_device_exec) << std::endl;
        }
    }
    else
    {
        char user_cmd = 0;
        std::future_status console_status = std::future_status::ready;
        std::future_status cracked_status;

        HonoursProject::Logger::info("...:::=== Cracking Results ===:::...\n\n");

        do
        {
            cracked_status = cracked_future.wait_for(std::chrono::milliseconds(10));

            if (console_status == std::future_status::ready)
            {
                console_future = std::async(std::launch::async, &ConsoleInput::run, console_task.get());
            }

            console_status = console_future.wait_for(std::chrono::milliseconds(10));

            if (console_status == std::future_status::ready)
            {
                process_input_command(cracker_task, console_future.get());
            }
        } while (cracker_task->getStatus() != HonoursProject::CrackerTask::Status::Aborted &&
            cracker_task->getStatus() != HonoursProject::CrackerTask::Status::Cracked &&
            cracker_task->getStatus() != HonoursProject::CrackerTask::Status::Idle);

        std::string plain_msg = cracked_future.get();

        if (!plain_msg.empty())
        {
            std::cout << std::endl << attack_input[0] << ": " << plain_msg << std::endl;
        }
        else
        {
            if (cracker_task->getStatus() == HonoursProject::CrackerTask::Status::Idle)
            {
                std::cout << std::endl << attack_input[0] << ": " << "Not found!" << std::endl;
            }
        }
    }

    return 0;
}
 
void Program::process_input_command(std::shared_ptr<HonoursProject::CrackerTask> cracker_task, char cmd)
{
    std::stringstream ss;
    ss.imbue(comma_locale);

    std::cout << std::endl;

    switch (cmd)
    {
    case 'p':
        {
            cracker_task->setStatus(HonoursProject::CrackerTask::Status::Paused);

            std::cout << "Pause cracking process ..." << std::endl;
        }
        break;

    case 'r':
        {
            cracker_task->setStatus(HonoursProject::CrackerTask::Status::Running);

            std::cout << "Resume cracking process ..." << std::endl;
        }
        break;

    case 'q':
        {
            cracker_task->setStatus(HonoursProject::CrackerTask::Status::Aborted);

            std::cout << "Stopping cracking process ..." << std::endl;
        }
        break;

    case 's':
        {
            std::cout << std::setfill('.') << std::setw(25) << std::left << "Cracking.Status";
            std::cout << ": " << cracker_task->getStatus() << std::endl;

            std::cout << std::setfill('.') << std::setw(25) << std::left << "Hash.Target";
            std::cout << ": " << hash_msg << std::endl;

            std::cout << std::setfill('.') << std::setw(25) << std::left << "Hash.Type";
            std::cout << ": " << hash_factory->type() << std::endl;

            std::time_t start_time = std::chrono::system_clock::to_time_t(cracker_task->getTimeStart());

            std::cout << std::setfill('.') << std::setw(25) << std::left << "Time.Started";
            std::cout << ": " << std::put_time(std::gmtime(&start_time), "%D %T") << " " << format_display_time(cracker_task->getTimeRunning()) << std::endl;

            std::time_t finish_time = std::chrono::system_clock::to_time_t(cracker_task->getTimeFinish());

            std::cout << std::setfill('.') << std::setw(25) << std::left << "Time.Estimated";
            std::cout << ": " << std::put_time(std::gmtime(&finish_time), "%D %T") << " " << format_display_time(cracker_task->getTimeEstimated()) << std::endl;

            ss << std::setfill('.') << std::setw(25) << std::left << "Cracking.Progress";
            ss << ": " << cracker_task->getTotalMessageProgress() << " / " << cracker_task->getTotalMessageSize();
            std::cout << ss.str() << std::endl; ss.clear();

            double total_device_speed = 0.0, total_device_exec = 0.0;

            for (std::size_t device_pos = 0; device_pos < cracker_task->getDeviceNum(); device_pos++)
            {
                std::shared_ptr<HonoursProject::Device> device = cracker_task->getDeviceAt(device_pos);
                double device_speed = cracker_task->getDeviceSpeed(device_pos);
                double device_exec = cracker_task->getDeviceExec(device_pos);

                std::cout << std::setfill('.') << std::setw(25) << std::left << ("Speed.Device.#" + std::to_string(device->getId()));
                std::cout << ": " << format_display_speed(device_speed, device_exec) << std::endl;

                total_device_speed += device_speed;
                total_device_exec += device_exec;
            }

            if (cracker_task->getDeviceNum() > 1)
            {
                std::cout << std::setfill('.') << std::setw(25) << std::left << "Speed.Device.*";
                std::cout << ": " << format_display_speed(total_device_speed, total_device_exec) << std::endl;
            }
        }
        break;
    }

    std::cout << std::endl;
}

std::string Program::format_display_time(std::chrono::seconds time)
{
    std::stringstream ss;

    std::time_t tt = time.count();
    tm* gmt_tm = std::gmtime(&tt);

    ss << "(";

    int year = gmt_tm->tm_year - 70;

    if (year)
    {
        if (year < 10)
        {
            ss << std::put_time(gmt_tm, "%Y years %j days");
        }
        else
        {
            ss << "> 10 years";
        }
    }
    else if (gmt_tm->tm_yday)
    {
        ss << std::put_time(gmt_tm, "%j days %H hours");
    }
    else if (gmt_tm->tm_hour)
    {
        ss << std::put_time(gmt_tm, "%H hours %M mins");
    }
    else if (gmt_tm->tm_min)
    {
        ss << std::put_time(gmt_tm, "%M mins %S secs");
    }
    else
    {
        ss << std::put_time(gmt_tm, "%S secs");
    }

    ss << ")";

    return ss.str();
}

std::string Program::format_display_speed(double speed_time, double exec_time)
{
    std::stringstream ss;
    ss.imbue(comma_locale);

    char units[7] = { ' ', 'k', 'M', 'G', 'T', 'P', 'E' };

    std::uint32_t level = 0;

    speed_time *= 1000.0;

    while (speed_time > 99999)
    {
        speed_time /= 1000;

        level++;
    }

    if (level > 0)
    {
        ss << std::setprecision(2) << std::fixed << speed_time << " " << units[level] << "H/s ";
    }
    else
    {
        ss << std::setprecision(2) << std::fixed << speed_time << "H/s ";
    }

    ss << "(" << std::setprecision(2) << std::fixed << exec_time << " ms)";

    return ss.str();
}
