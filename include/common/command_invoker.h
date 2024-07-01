#ifndef TRANSACTIONS_INCLUDE_COMMON_COMMAND_INVOKER_H_
#define TRANSACTIONS_INCLUDE_COMMON_COMMAND_INVOKER_H_

#include <sstream>
#include <functional>
#include "common/command.h"

namespace s21
{

template<class Container>
class CommandInvoker
{
public:
    bool SetCommand(const std::string& line)
    {
        std::istringstream iss(line);
        std::string cmd;
        
        iss >> cmd;
        std::transform(cmd.begin(), cmd.end(), cmd.begin(), [](auto c)
        {
            return std::toupper(c);
        });
        
        if (cmd == "SET")
        {
            command_ = std::make_unique<cmd::SetCommand<Container>>(iss);
        }
        else if (cmd == "GET")
        {
            command_ = std::make_unique<cmd::GetCommand<Container>>(iss);
        }
        else if (cmd == "EXISTS")
        {
            command_ = std::make_unique<cmd::ExistsCommand<Container>>(iss);
        }
        else if (cmd == "DEL")
        {
            command_ = std::make_unique<cmd::DeleteCommand<Container>>(iss);
        }
        else if (cmd == "UPDATE")
        {
            command_ = std::make_unique<cmd::UpdateCommand<Container>>(iss);
        }
        else if (cmd == "KEYS")
        {
            command_ = std::make_unique<cmd::KeysCommand<Container>>();
        }
        else if (cmd == "RENAME")
        {
            command_ = std::make_unique<cmd::RenameCommand<Container>>(iss);
        }
        else if (cmd == "TTL")
        {
            command_ = std::make_unique<cmd::TTLCommand<Container>>(iss);
        }
        else if (cmd == "FIND")
        {
            command_ = std::make_unique<cmd::FindCommand<Container>>(iss);
        }
        else if (cmd == "SHOWALL")
        {
            command_ = std::make_unique<cmd::ShowAllCommand<Container>>();
        }
        else if (cmd == "UPLOAD")
        {
            command_ = std::make_unique<cmd::UploadCommand<Container>>(iss);
        }
        else if (cmd == "EXPORT")
        {
            command_ = std::make_unique<cmd::ExportCommand<Container>>(iss);
        }
        else
        {
            return false;
        }
        
        return true;
    }
    
    void ExecuteCommand(Container& storage)
    {
        if (command_)
        {
            try
            {
                command_->Execute(storage);
            }
            catch (const std::exception& e)
            {
                std::cerr << e.what() << std::endl;
            }
        }
    }

private:
    std::unique_ptr<cmd::Command<Container>> command_;
};

} // namespace s21

#endif // TRANSACTIONS_INCLUDE_COMMON_COMMAND_INVOKER_H_
