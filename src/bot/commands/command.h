#pragma once

#include <aegis.hpp>

struct CommandInfo {
    std::string name;
    std::vector<std::string> aliases;
    std::string description;
    std::vector<std::string> options;
};

struct MessageInfo {
    aegis::snowflake messageId;
    aegis::snowflake channelId;
    aegis::snowflake guildId;
    aegis::snowflake userId;
    bool isDm;
};

class ClassroomBot;

class Command {
public:
    explicit Command(std::shared_ptr<ClassroomBot> classroomBot);

    virtual void call(std::vector<std::string> parameters, MessageInfo current) {
        _current = current;
    }

    virtual CommandInfo getCommandInfo() = 0;

protected:
    std::shared_ptr<spdlog::logger> _log;
    std::shared_ptr<aegis::core> _aegisCore;
    std::shared_ptr<ClassroomBot> _bot;
    MessageInfo _current;

};