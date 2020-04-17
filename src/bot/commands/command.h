#pragma once

#include <aegis.hpp>
#include "../bot.h"

struct CommandInfo {
    std::vector<std::string> aliases;
    std::string description;
    std::vector<std::string> options;
    std::string permsMessage;
};

struct CurrentCommand {
    aegis::snowflake messageId;
    aegis::snowflake channelId;
    aegis::snowflake guildId;
    aegis::snowflake userId;
};

class Command {
public:
    explicit Command(ClassroomBot* classroomBot)
    : _log(classroomBot->_log)
    , _aegisCore(classroomBot->_aegisCore)
    {}

    virtual void call(std::vector<std::string> parameters, CurrentCommand current) { // call the command
        _current = current;
    }

    virtual CommandInfo getCommandInfo() = 0;
    virtual bool checkPermissions(aegis::permission channelPermissions) = 0;


    virtual ~Command() {}

protected:
    std::shared_ptr<spdlog::logger> _log;
    std::shared_ptr<aegis::core> _aegisCore;
    CurrentCommand _current;

};