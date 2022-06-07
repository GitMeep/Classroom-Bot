#pragma once

#include <dpp/dpp.h>
#include <string>
#include <vector>
#include <chrono>

class Command;
class Config;
class SettingsRepo;
class QuestionRepo;
class HandRepo;
class MuteRepo;
class DB;
class Localization;

namespace spdlog {
    class logger;
};

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
    static void init();
    static void registerCommand(Command* command);
    static bool run();

    static void log(const dpp::loglevel& ll, const std::string& message);

    static dpp::cluster& cluster();

private:
    static void onSlashCommand(const dpp::slashcommand_t& event);
    static void onSelectClick(const dpp::select_click_t& event);
    static void onButtonClick(const dpp::button_click_t& event);
    static void onFormSubmit(const dpp::form_submit_t& event);
    static void onUserContext(const dpp::user_context_menu_t& event);
    static void onMessageContext(const dpp::message_context_menu_t& event);

    static void onLog(const dpp::log_t& event);
    static void logMessage(const dpp::loglevel& ll, const std::string& message);

    static void updatePresence(dpp::timer timer);

    static dpp::cluster* m_Cluster;
    static std::vector<Command*> m_Commands;

    static std::chrono::system_clock::time_point m_StartupTime;

    static unsigned char m_PresenceState;
    static bool m_Initialized;
    static bool m_LogInitialized;

    static std::shared_ptr<spdlog::logger> m_Logger;
};
