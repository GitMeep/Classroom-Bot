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

class ClassroomBot;

class Command {
public:
    explicit Command(std::shared_ptr<ClassroomBot> classroomBot)
    : _log(spdlog::get("console"))
    , _aegisCore(classroomBot->_aegisCore)
    , _bot(classroomBot)
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
    std::shared_ptr<ClassroomBot> _bot;
    CurrentCommand _current;

};