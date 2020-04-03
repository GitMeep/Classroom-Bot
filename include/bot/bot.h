#pragma once

#include <aegis.hpp>

#include <bot/commands/command.h>

class questionsbot {
public:
    questionsbot(std::shared_ptr<spdlog::logger> logger);
    void onMessage(aegis::gateway::events::message_create message);
    void registerCommand(Command* command);

private:
    std::vector<std::string> parseCommand(std::string message, char prefix);
    std::unique_ptr<Command> getCommand(std::string);

    std::shared_ptr<spdlog::logger> _log;
    std::map<std::string, std::string> _aliases;
    std::map<std::string, std::shared_ptr<Command>> _commands;
};