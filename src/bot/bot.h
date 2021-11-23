#pragma once

#include <dpp/dpp.h>
#include <spdlog/spdlog.h>

#include <bot/commands/commandhandler/commandHandler.h>

class Command;
class Config;
class SettingsRepository;
class QuestionRepository;
class HandRepository;
class MuteRepository;
class DB;
class Localization;

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
    void onMessage(const dpp::message_create_t& message);
    void init();
    bool run();

    static ClassroomBot& getBot();
    static std::shared_ptr<Config> getConfig();
    static std::shared_ptr<dpp::cluster> getCluster();
    static std::shared_ptr<SettingsRepository> getSettingsRepo();
    static std::shared_ptr<QuestionRepository> getQuestionRepo();
    static std::shared_ptr<HandRepository> getHandRepo();
    static std::shared_ptr<MuteRepository> getMuteRepo();
    static std::shared_ptr<DB> getDatabase();
    static std::shared_ptr<spdlog::logger> getLog();
    static std::shared_ptr<CommandHandler> getCommandHandler();
    static std::shared_ptr<Localization> getLocalization();

private:
    void updatePresence();

    std::shared_ptr<Config> m_Config;
    std::shared_ptr<dpp::cluster> m_Cluster;
    std::shared_ptr<SettingsRepository> m_SettingsRepo;
    std::shared_ptr<QuestionRepository> m_QuestionRepo;
    std::shared_ptr<HandRepository> m_HandRepo;
    std::shared_ptr<MuteRepository> m_MuteRepo;
    std::shared_ptr<DB> m_Database;
    std::shared_ptr<CommandHandler> m_CommandHandler;
    std::shared_ptr<Localization> m_Localization;

    std::shared_ptr<spdlog::logger> m_Log;

    std::chrono::system_clock::time_point m_StartupTime;

    unsigned char m_PresenceState = 0;
    bool m_Initialized = false;
};