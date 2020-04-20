#pragma once

#include <aegis.hpp>
#include <chrono>
#include "persistence/persistence.h"
#include "config/config.h"

class Command;

class ClassroomBot {
public:
    explicit ClassroomBot(std::string token, std::shared_ptr<spdlog::logger> log, std::shared_ptr<Config> config);
    void onMessage(aegis::gateway::events::message_create message);
    void onGuildCreate(aegis::gateway::events::guild_create obj);
    void registerCommand(Command* command);
    void run();
    
    std::shared_ptr<Config> _config;
    std::shared_ptr<SettingsRepo> _settingsRepo;
    std::shared_ptr<aegis::core> _aegisCore;
    std::shared_ptr<Persistence> _persistence;

private:
    std::vector<std::string> parseCommand(std::string& message, char prefix);
    void printHelp(aegis::snowflake channelId);
    void updatePresence();
    bool tryUpdatePresence();

    std::shared_ptr<spdlog::logger> _log;
    
    std::map<std::string, std::string> _aliases;
    std::map<std::string, std::shared_ptr<Command>> _commands;
    std::chrono::system_clock::time_point _lastPresenceUpdate;
    bool _shouldUpdatePresence = false;
};