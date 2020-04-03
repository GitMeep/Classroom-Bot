#pragma once

#include <aegis.hpp>

struct CommandInfo {
    std::vector<std::string> aliases;
    std::string description;
    std::vector<std::string> options;
};

struct CurrentCommand {
    aegis::snowflake messageId;
    aegis::snowflake channelId;
    aegis::snowflake guildId;
    aegis::snowflake userId;
};

class Command {
public:
    Command(std::shared_ptr<spdlog::logger> log, std::shared_ptr<aegis::core> aegisCore)
    : _log(log)
    , _aegisCore(aegisCore)
    {}
    virtual void call(std::vector<std::string> parameters, CurrentCommand current) { // call the command
        _current = current;
    }

    virtual CommandInfo getCommandInfo() = 0; // called on command registration


    virtual ~Command() {}

protected:
    std::shared_ptr<spdlog::logger> _log;
    std::shared_ptr<aegis::core> _aegisCore;
    CurrentCommand _current;

};