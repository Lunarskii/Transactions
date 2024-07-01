#include "common/cli.h"

namespace s21
{

int CLI::Run()
{
    while (MainSelector_())
    {
        CleanInputStream_();
    }

    return 0;
}

void CLI::CleanInputStream_()
{
    if (std::cin.fail())
    {
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    }
}

bool CLI::MainSelector_()
{
    static const auto GetMainMenuSelection = []()
    {
        int chooser = 0;
        std::cout << "\t1. Select the storage type\n"
                     "\t2. Select the function to call\n"
                     "\t3. Research\n"
                     "\t0. Exit\n"
                     ">> ";
        std::cin >> chooser;
        
        return std::cin.fail() ? -1 : chooser;
    };
    
    try
    {
        switch (GetMainMenuSelection())
        {
            case 1:
                while (SelectStorageType_())
                {
                    CleanInputStream_();
                };
                break;
            case 2:
                if (!storage_)
                {
                    std::cout << "Storage is not selected" << std::endl;
                }
                else
                {
                    while (SelectFunction_()) {}
                }
                break;
            case 3:
                Research_();
                break;
            case 0:
                return false;
            default:
                std::cout << "\tTry again...\n";
        }
    }
    catch (const std::exception& e)
    {
        std::cout << e.what() << std::endl;
    }
    
    return true;
}

bool CLI::SelectStorageType_()
{
    static const auto GetStorageTypeSelection = []()
    {
        int chooser = 0;
        std::cout << "\t1. Hash table\n"
                     "\t2. Self-balancing binary search tree\n"
                     "\t3. B+ tree\n"
                     "\t0. Back\n"
                     ">> ";
        std::cin >> chooser;
        
        return std::cin.fail() ? -1 : chooser;
    };
    
    switch (GetStorageTypeSelection())
    {
        case 1:
            storage_ = std::make_unique<wrapper_type>(new hash_table);
            return false;
        case 2:
            storage_ = std::make_unique<wrapper_type>(new rb_tree);
            return false;
        case 3:
            storage_ = std::make_unique<wrapper_type>(new b_plus_tree);
            return false;
        case 0:
            return false;
        default:
            std::cout << "\tTry again...\n";
    }
    
    return true;
}

bool CLI::SelectFunction_()
{
    static const auto GetStorageFunctionSelection = []()
    {
        std::string line;
        std::cout << "Available commands:\n"
                     "\tSET <key> <last_name> <first_name> <year> <city> <coins> EX <time>\n"
                     "\tGET <key>\n"
                     "\tEXISTS <key>\n"
                     "\tDEL <key>\n"
                     "\tUPDATE <key> <last_name> <first_name> <year> <city> <coins>\n"
                     "\tKEYS\n"
                     "\tRENAME <current_key> <new_key>\n"
                     "\tTTL <key>\n"
                     "\tFIND <last_name> <first_name> <year> <city> <coins>\n"
                     "\tSHOWALL\n"
                     "\tUPLOAD <path/to/file>\n"
                     "\tEXPORT <path/to/file>\n"
                     "0. Back\n"
                     ">> ";
        std::getline(std::cin, line);
        
        return line;
    };

    auto line = GetStorageFunctionSelection();
    if (line == "0")
    {
        return false;
    }
    else
    {
        if (command_->SetCommand(line))
        {
            command_->ExecuteCommand(*storage_);
        }
        else
        {
            std::cout << "The command does not exist" << std::endl;
        }
    }

    return true;
}

void CLI::Research_()
{
    std::size_t starting_num_elements;
    std::size_t num_times;

    std::cout << "Enter the starting number of elements in the container." << std::endl;
    std::cin >> starting_num_elements;
    std::cout << "Enter the number of iterations for each action." << std::endl;
    std::cin >> num_times;

    if (!std::cin.fail())
    {
        std::unique_ptr<wrapper_type> ht = std::make_unique<wrapper_type>(new hash_table);
        std::unique_ptr<wrapper_type> sbt = std::make_unique<wrapper_type>(new rb_tree);
        Research ht_research(ht.get());
        Research sbt_research(sbt.get());

        std::cout << "HashTable: " << ht_research.Run(starting_num_elements, num_times) << "ms" << std::endl;
        std::cout << "RBTree: " << sbt_research.Run(starting_num_elements, num_times) << "ms" << std::endl;
    }
    else
    {
        std::cout << "\tTry again...\n";
    }
}

} // namespace s21
