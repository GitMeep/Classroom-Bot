#pragma once

#include <dpp/dpp.h>
#include <spdlog/spdlog.h>

class Command;
class Config;
class SettingsRepo;
class QuestionRepo;
class HandRepo;
class MuteRepo;
class DB;
class Localization;

namespace odb {
    namespace core {
        class database;
    };
};

// convenience macros for logging
#define LOG_TRACE(message) ClassroomBot::log(dpp::ll_trace, message)
#define LOG_DEBUG(message) ClassroomBot::log(dpp::ll_debug, message)
#define LOG_INFO(message) ClassroomBot::log(dpp::ll_info, message)
#define LOG_WARN(message) ClassroomBot::log(dpp::ll_warning, message)
#define LOG_ERROR(message) ClassroomBot::log(dpp::ll_error, message)
#define LOG_CRITICAL(message) ClassroomBot::log(dpp::ll_critical, message)

class ClassroomBot {
public:
    static void registerCommand(Command* command);
    static void onCommand(const dpp::slashcommand_t& event);
    static void init();
    static bool run();

    static void log(const dpp::loglevel& ll, const std::string& message);

    static dpp::cluster&        cluster();

private:
    static void updatePresence(dpp::timer timer);

    static dpp::cluster* m_Cluster;

    static std::chrono::system_clock::time_point m_StartupTime;

    static unsigned char m_PresenceState;
    static bool m_Initialized;
};
