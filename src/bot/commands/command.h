#pragma once

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
    Command();

    virtual void call(const std::vector<std::string>& parameters, MessageInfo* current) = 0;

    virtual CommandInfo getCommandInfo() = 0;

protected:
    std::shared_ptr<spdlog::logger> _log;
    std::shared_ptr<aegis::core> _aegisCore;
    ClassroomBot* _bot;

};