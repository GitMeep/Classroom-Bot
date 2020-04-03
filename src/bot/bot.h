#pragma once

#include <aegis.hpp>

#include "commands/command.h"

class ClassroomBot {
public:
    ClassroomBot(std::shared_ptr<spdlog::logger> logger, aegis::core* core);
    void onMessage(aegis::gateway::events::message_create message);
    void registerCommand(Command* command);

private:
    std::vector<std::string> parseCommand(std::string message, char prefix);
    std::unique_ptr<Command> getCommand(std::string);
    void printHelp(aegis::snowflake channelId);

    std::shared_ptr<spdlog::logger> _log;
    std::shared_ptr<aegis::core> _aegisCore;
    std::map<std::string, std::string> _aliases;
    std::map<std::string, std::shared_ptr<Command>> _commands;
};