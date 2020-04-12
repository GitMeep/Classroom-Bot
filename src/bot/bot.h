#pragma once

#include <aegis.hpp>
#include <chrono>

#include "commands/command.h"

class ClassroomBot {
public:
    ClassroomBot(std::shared_ptr<spdlog::logger> logger, aegis::core* core);
    void onMessage(aegis::gateway::events::message_create message);
    void onGuildCreate(aegis::gateway::events::guild_create obj);
    void registerCommand(Command* command);

private:
    std::vector<std::string> parseCommand(std::string message, char prefix);
    std::unique_ptr<Command> getCommand(std::string);
    void printHelp(aegis::snowflake channelId);
    void updatePresence();
    bool tryUpdatePresence();

    std::shared_ptr<spdlog::logger> _log;
    std::shared_ptr<aegis::core> _aegisCore;
    std::map<std::string, std::string> _aliases;
    std::map<std::string, std::shared_ptr<Command>> _commands;
    std::chrono::system_clock::time_point _lastPresenceUpdate;
    bool _shouldUpdatePresence = false;
};