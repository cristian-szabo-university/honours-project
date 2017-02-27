#include "HashCracker.hpp"

#include "docopt.h"

#include "Program.hpp"

#include "ConsoleInput.hpp"

const char Program::USAGE[] =
R"(
Usage:
    my_program (-m HMSG -a MODE -c NAME -k MASK) [--benchmark]

Options:
    -m HMSG --hash-message=HMSG  specify hashed messaged to crack
    -a MODE --attack=MODE        specify attack mode
    -c NAME --hash-func=NAME     specify the crypthographic hash function
    -k MASK --mask=MASK          specify bruteforce attack mask
    -d FILE --dict-file=FILE     specify dictionary file path
    --benchmark                  specify benchmark mode
)";

char comma_num_punct::do_thousands_sep() const
{
    return ',';
}

std::string comma_num_punct::do_grouping() const
{
    return "\03";
}

Program::Program(std::vector<std::string> cli)
{
    args = docopt::docopt(USAGE, cli, true, "HashApp 1.0");

    comma_locale = std::locale(std::locale(), new comma_num_punct());
}

int Program::run()
{
    std::future<char> console_future;
    std::future<std::string> cracked_future;
    std::shared_ptr<ConsoleInput> console_task = std::make_shared<ConsoleInput>();

    std::string hash_msg = args["--hash-message"].asString();

    std::string hashFunc = args["--hash-func"].asString();

    HonoursProject::HashCracker::HashFunc hash_func = HonoursProject::HashCracker::HashFunc::Invalid;

    if (hashFunc.compare("md5") == 0)
    {
        hash_func = HonoursProject::HashCracker::HashFunc::MD5;
    }
    else if (hashFunc.compare("sha1") == 0)
    {
        hash_func = HonoursProject::HashCracker::HashFunc::SHA1;
    }

    bool benchmark = args["--benchmark"].asBool();

    hash_cracker = std::make_shared<HonoursProject::HashCracker>(hash_msg, hash_func, benchmark);

    std::string attackMode = args["--attack"].asString();

    std::string attack_input;
    HonoursProject::HashCracker::AttackMode attack_mode = HonoursProject::HashCracker::AttackMode::Invalid;

    if (attackMode.compare("bruteforce") == 0)
    {
        attack_input = args["--mask"].asString();

        attack_mode = HonoursProject::HashCracker::AttackMode::Bruteforce;
    }
    else if (attackMode.compare("dictionary") == 0)
    {
        attack_input = args["--dict-file"].asString();

        attack_mode = HonoursProject::HashCracker::AttackMode::Dictionary;
    }

    cracked_future = hash_cracker->executeAttack(attack_mode, attack_input);

    char user_cmd = 0;
    std::future_status console_status = std::future_status::ready;
    std::future_status cracked_status;

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
            process_input_command(console_future.get());
        }
    }
    while (hash_cracker->getStatus() != HonoursProject::HashCracker::Status::Aborted &&
        hash_cracker->getStatus() != HonoursProject::HashCracker::Status::Cracked &&
        hash_cracker->getStatus() != HonoursProject::HashCracker::Status::Idle);

    std::string plain_msg = cracked_future.get();

    if (!plain_msg.empty())
    {
        std::cout << std::endl << hash_msg << ": " << plain_msg << std::endl;
    }
    else
    {
        if (hash_cracker->getStatus() == HonoursProject::HashCracker::Status::Idle)
        {
            std::cout << std::endl << hash_msg << ": " << "Not found!" << std::endl;
        }
    }

    return 0;
}
 
void Program::process_input_command(char cmd)
{
    std::cout << std::endl;

    switch (cmd)
    {
    case 'p':
        hash_cracker->setStatus(HonoursProject::HashCracker::Status::Paused);
        break;

    case 'r':
        hash_cracker->setStatus(HonoursProject::HashCracker::Status::Running);
        break;

    case 'q':
        hash_cracker->setStatus(HonoursProject::HashCracker::Status::Aborted);
        break;

    case 's':
        {
            std::cout << std::setfill('.') << std::setw(25) << std::left << "Hash.Target";
            std::cout << ": " << hash_cracker->getHashMsg() << std::endl;

            std::cout << std::setfill('.') << std::setw(25) << std::left << "Hash.Type";
            std::cout << ": " << hash_cracker->getHashFunc() << std::endl;

            std::time_t start_time = std::chrono::system_clock::to_time_t(hash_cracker->getTimeStart());

            std::cout << std::setfill('.') << std::setw(25) << std::left << "Time.Started";
            std::cout << ": " << std::put_time(std::gmtime(&start_time), "%D %T") << " " << format_display_time(hash_cracker->getTimeRunning()) << std::endl;

            std::time_t finish_time = std::chrono::system_clock::to_time_t(hash_cracker->getTimeFinish());

            std::cout << std::setfill('.') << std::setw(25) << std::left << "Time.Estimated";
            std::cout << ": " << std::put_time(std::gmtime(&finish_time), "%D %T") << " " << format_display_time(hash_cracker->getTimeEstimated()) << std::endl;

            std::cout << std::setfill('.') << std::setw(25) << std::left << "Progress";
            std::cout << ": " << hash_cracker->getMessageProgress() << " / " << hash_cracker->getMessageTotal() << std::endl;

            for (std::size_t device_pos = 0; device_pos < hash_cracker->getDeviceNum(); device_pos++)
            {
                std::cout << std::setfill('.') << std::setw(25) << std::left << ("Speed.Device.#" + std::to_string(device_pos));
                std::cout << ": " << format_display_speed(hash_cracker->getDeviceSpeed(device_pos), hash_cracker->getDeviceExec(device_pos)) << std::endl;
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

    if (gmt_tm->tm_year - 70)
    {
        ss << std::put_time(gmt_tm, "%Y years %j days");
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

    // ms to sec
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
