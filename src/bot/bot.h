#pragma once

#include <aegis.hpp>
#include <chrono>
#include "persistence/persistence.h"
#include "commands/commandhandler/commandHandler.h"
#include "config/config.h"

class Command;

class ClassroomBot {
public:
    explicit ClassroomBot(std::string token, std::shared_ptr<spdlog::logger> log, std::shared_ptr<Config> config);
    void registerCommand(Command* command);
    void onMessage(aegis::gateway::events::message_create message);
    void onGuildCreate(aegis::gateway::events::guild_create obj);
    void run();
    
    std::shared_ptr<Config> _config;
    std::shared_ptr<SettingsRepo> _settingsRepo;
    std::shared_ptr<aegis::core> _aegisCore;
    std::shared_ptr<Persistence> _persistence;
    CommandHandler _commandHandler;

private:
    void updatePresence();
    bool tryUpdatePresence();

    std::shared_ptr<spdlog::logger> _log;
    
    std::chrono::system_clock::time_point _lastPresenceUpdate;
    bool _shouldUpdatePresence = false;
};