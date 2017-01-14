#include "HashCrack.hpp"

#include "docopt.h"

#include "Program.hpp"

#include "ConsoleInput.hpp"

const char Program::USAGE[] =
R"(
Usage:
    my_program (-m HMSG -a MODE -c NAME -k MASK) [--increment] [--benchmark]

Options:
    -m HMSG --hash-message=HMSG  specify hashed messaged to crack
    -a MODE --attack=MODE        specify attack mode
    -c NAME --hash-func=NAME     specify the crypthographic hash function
    -k MASK --mask=MASK          specify bruteforce attack mask
    -d FILE --dict-file=FILE     specify dictionary file path
    --benchmark                  specify benchmark mode
    --increment                  specify mask range to start from 1
)";

Program::Program(std::vector<std::string> cli)
{
    args = docopt::docopt(USAGE, cli, true, "HashApp 1.0");
}

int Program::run()
{
    std::future<char> console_result;
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

    hash_cracker->executeAttack(attack_mode, attack_input);

    char user_cmd = 0;
    std::future_status console_status = std::future_status::ready;
    
    do
    {
        if (console_status == std::future_status::ready)
        {
            console_result = std::async(std::launch::async, &ConsoleInput::run, console_task.get());
        }

        console_status = console_result.wait_for(std::chrono::milliseconds(10));

        if (console_status == std::future_status::ready)
        {
            processUserCommand(console_result.get());
        }
    }
    while (hash_cracker->getStatus() != HonoursProject::HashCracker::Status::Aborted &&
        hash_cracker->getStatus() != HonoursProject::HashCracker::Status::Cracked &&
        hash_cracker->getStatus() != HonoursProject::HashCracker::Status::Idle);

    std::string plain_msg = hash_cracker->getPlainMsg();

    std::cout << std::endl;

    if (!plain_msg.empty())
    {
        std::cout << hash_msg << ": " << plain_msg << std::endl;
    }
    else
    {
        if (hash_cracker->getStatus() == HonoursProject::HashCracker::Status::Idle)
        {
            std::cout << hash_msg << ": " << "Not found!" << std::endl;
        }
    }

    std::cout << std::endl;

    return 0;
}

void Program::processUserCommand(char cmd)
{
    std::cout << std::endl;

    switch (cmd)
    {
    case 'p':
        hash_cracker->pause();
        break;

    case 'r':
        hash_cracker->resume();
        break;

    case 'q':
        hash_cracker->quit();
        break;

    case 's':
        {
            std::cout << std::setfill('.') << std::setw(25) << std::left << "Hash.Target";
            std::cout << ": " << hash_cracker->getHashMsg() << std::endl;

            std::cout << std::setfill('.') << std::setw(25) << std::left << "Hash.Type";
            std::cout << ": " << hash_cracker->getHashFunc() << std::endl;

            std::cout << std::setfill('.') << std::setw(25) << std::left << "Time.Started";
            std::cout << ": " << hash_cracker->getTimeStart() << std::endl;

            std::cout << std::setfill('.') << std::setw(25) << std::left << "Time.Estimated";
            std::cout << ": " << hash_cracker->getTimeEstimate() << std::endl;

            std::cout << std::setfill('.') << std::setw(25) << std::left << "Progress";
            std::cout << ": " << hash_cracker->getProgress() << std::endl;

            for (std::size_t device_pos = 0; device_pos < hash_cracker->getDeviceNum(); device_pos++)
            {
                std::cout << std::setfill('.') << std::setw(25) << std::left << ("Speed.Device.#" + std::to_string(device_pos));
                std::cout << ": " << hash_cracker->getDeviceSpeed(device_pos) << std::endl;
            }
        }
        break;
    }

    std::cout << std::endl;
}
