#pragma once

#include <bot/commands/commandhandler/commandHandler.h>

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
    void init();
    bool run();

    static ClassroomBot& get();

    std::shared_ptr<Config> getConfig();
    std::shared_ptr<aegis::core> getAegis();
    std::shared_ptr<SettingsRepository> getSettingsRepo();
    std::shared_ptr<QuestionRepository> getQuestionRepo();
    std::shared_ptr<HandRepository> getHandRepo();
    std::shared_ptr<MuteRepository> getMuteRepo();
    std::shared_ptr<DB> getDatabase();
    std::shared_ptr<spdlog::logger> getLog();
    std::shared_ptr<CommandHandler> getCommandHandler();

private:
    void updatePresence();

    std::shared_ptr<Config> m_Config;
    std::shared_ptr<aegis::core> m_AegisCore;
    std::shared_ptr<SettingsRepository> m_SettingsRepo;
    std::shared_ptr<QuestionRepository> m_QuestionRepo;
    std::shared_ptr<HandRepository> m_HandRepo;
    std::shared_ptr<MuteRepository> m_MuteRepo;
    std::shared_ptr<DB> m_Database;
    std::shared_ptr<CommandHandler> m_CommandHandler;

    std::shared_ptr<spdlog::logger> m_Log;
    std::unique_ptr<asio::steady_timer> m_PresenceTimer;

    std::chrono::system_clock::time_point m_StartupTime;

    unsigned char m_PresenceState = 0;
    bool m_Initialized = false;
};