#pragma once

#include "commands/commandhandler/commandHandler.h"

class Command;
class Config;
class SettingsRepository;
class QuestionRepository;
class HandRepository;
class MuteRepository;
class DB;

namespace aegis {
    class core;
    class snowflake;
    namespace gateway {
        namespace events {
            struct message_create;
            struct guild_create;
        };
    };
};

namespace odb {
    namespace core {
        class database;
    };
};

class ClassroomBot {
public:
    void registerCommand(Command* command);
    void onMessage(aegis::gateway::events::message_create message);
    void init(const std::string& token, const std::shared_ptr<Config>& config);
    bool run();

    static ClassroomBot& get();
    
    std::shared_ptr<Config> _config;
    std::shared_ptr<aegis::core> _aegisCore;
    std::shared_ptr<SettingsRepository> _settingsRepo;
    std::shared_ptr<QuestionRepository>_questionRepo;
    std::shared_ptr<HandRepository> _handRepo;
    std::shared_ptr<MuteRepository> m_MuteRepo;
    CommandHandler _commandHandler;

private:
    void updatePresence();

    std::shared_ptr<spdlog::logger> _log;
    std::shared_ptr<DB> _database;
    std::unique_ptr<asio::steady_timer> m_PresenceTimer;

    unsigned char m_PresenceState = 0;
    bool m_Initialized = false;
};