#include <iostream>
#include <vector>

#include <bot/commands/command.h>
#include <bot/config/config.h>
#include <bot/persistence/db.h>
#include <bot/persistence/repo/settingsRepo.h>
#include <bot/persistence/repo/handRepo.h>
#include <bot/persistence/repo/questionRepo.h>
#include <bot/persistence/repo/muteRepo.h>
#include <bot/localization/localization.h>

#include <Poco/Net/HTTPSClientSession.h>
#include <Poco/Net/HTTPRequest.h>
#include <Poco/Net/HTTPResponse.h>

#include <bot/bot.h>
#include <dpp/dpp.h>

enum ParserException {
    NOT_A_COMMAND,
    EMPTY_COMMAND
};

// initialize static members
dpp::cluster*                           ClassroomBot::m_Cluster;
std::chrono::system_clock::time_point   ClassroomBot::m_StartupTime;
unsigned char                           ClassroomBot::m_PresenceState = 0;
bool                                    ClassroomBot::m_Initialized = false;
std::vector<Command*>                   ClassroomBot::m_Commands;

void ClassroomBot::init() {
    m_StartupTime = std::chrono::system_clock::now();

    /*
    m_Log = spdlog::stdout_color_mt("classroombot");
    m_Log->set_pattern("%^%Y-%m-%d %H:%M:%S.%e [%L] [ClassroomBot] [th#%t]%$ : %v");
    m_Log->set_level(spdlog::level::trace);
    */

    try {
        Config::loadFromFile("config.json");
    } catch (std::runtime_error& e) {
        LOG_ERROR(std::string(e.what()));
        return;
    }

    if(!Config::isLoaded()) {
        throw std::runtime_error("Invalid config supplied to ClassroomBot!");
    }

    Localization::init();
    Encryption::init();
    DB::init();

    std::string token = Config::get()["bot"]["token"];
    m_Cluster = new dpp::cluster(token);
    m_Cluster->on_log(dpp::utility::cout_logger());
    m_Cluster->log(dpp::ll_info, "Starting ClassroomBot version " + std::string(BOT_VERSION));

    m_Cluster->on_slashcommand(ClassroomBot::onSlashCommand);
    m_Cluster->on_select_click(ClassroomBot::onSelectClick);
    m_Cluster->on_button_click(ClassroomBot::onButtonClick);
    m_Cluster->on_form_submit(ClassroomBot::onFormSubmit);
    m_Cluster->on_user_context_menu(ClassroomBot::onUserContext);
    m_Cluster->on_message_context_menu(ClassroomBot::onMessageContext);

    m_Cluster->on_ready([](const dpp::ready_t& event) {
        if (!dpp::run_once<struct register_bot_commands>()) return;
        if(m_Commands.size() == 0) return;

        std::vector<dpp::slashcommand> slashCommands;
        for(unsigned int i = 0; i < m_Commands.size(); i++) {
            for(dpp::slashcommand command : m_Commands[i]->spec().commands)
                slashCommands.push_back(command.set_application_id(m_Cluster->me.id));
        }

        #ifdef DEBUG
        m_Cluster->guild_bulk_command_create(slashCommands, 705355899400880212);
        #else
        m_Cluster->global_bulk_command_create(slashCommands);
        #endif

        updatePresence(0);
        cluster().start_timer(ClassroomBot::updatePresence, 60U);
    });

    m_Initialized = true;
}

bool ClassroomBot::run() {
    if(!m_Initialized) return m_Initialized;

    m_Cluster->start(false);

    return true;
}

void ClassroomBot::log(const dpp::loglevel& ll, const std::string& message) {
    if(m_Initialized) m_Cluster->log(ll, message);
}

void ClassroomBot::registerCommand(Command* command) {
   m_Commands.emplace_back(command);
}

void ClassroomBot::onSlashCommand(const dpp::slashcommand_t& event) {
    // LOG_DEBUG(event.raw_event);
    
    for(Command* command : m_Commands) {
        const Command::CommandSpec& spec = command->spec();

        for(const dpp::slashcommand& slashCommand : spec.commands) {
            if(slashCommand.type != dpp::ctxm_chat_input) continue;

            if(slashCommand.name == event.command.get_command_name()) {
                command->command(CommandContext(event, CommandContext::SlashCommand));
                return;
            }
        }
    }
    LOG_WARN("Unknown command: \"" + event.command.get_command_name() + "\" issued by " + event.command.usr.username + " in " + std::to_string(event.command.guild_id));
}

void ClassroomBot::onSelectClick(const dpp::select_click_t& event) {
    // TODO
}

void ClassroomBot::onButtonClick(const dpp::button_click_t& event) {
    // TODO
}

void ClassroomBot::onFormSubmit(const dpp::form_submit_t& event) {
    // TODO
}

void ClassroomBot::onUserContext(const dpp::user_context_menu_t& event) {
    // TODO
}

void ClassroomBot::onMessageContext(const dpp::message_context_menu_t& event) {
    // TODO
}

void ClassroomBot::updatePresence(dpp::timer timer) {
    LOG_INFO("Uptime: " + std::to_string(m_Cluster->uptime().hours) + " hours");

    unsigned int guildCount = 0;
    for(auto shard : m_Cluster->get_shards()) {
        guildCount += shard.second->get_guild_count();
    }

    switch (m_PresenceState) {
        case 0:
            m_Cluster->set_presence(dpp::presence(dpp::presence_status::ps_online, dpp::activity_type::at_listening, "?help"));
            break;
        
        case 1:
            m_Cluster->set_presence(dpp::presence(dpp::presence_status::ps_online, dpp::activity_type::at_custom, std::to_string(guildCount) + " servers"));
            break;

        case 2:
            m_Cluster->set_presence(dpp::presence(dpp::presence_status::ps_online, dpp::activity_type::at_game, "V" + std::string(BOT_VERSION)));
            break;

        default:
            break;
    }

    m_PresenceState++;
    m_PresenceState = m_PresenceState % 3;

    HandRepo::expire();
    QuestionRepo::expire();

    if(guildCount == 0) return;

    if(Config::get()["topgg"]["enable"] == "true") {
        std::string topggToken = Config::get()["topgg"]["token"];
        std::string topggId = Config::get()["topgg"]["bot_id"];

        Poco::Net::HTTPSClientSession session("top.gg", 443);
        Poco::Net::HTTPRequest req("POST", "/api/bots/" + topggId + "/stats");

        req.setCredentials("", topggToken);
        req.setContentType("application/json");

        std::string body = "{\"server_count\": " + std::to_string(guildCount) + "}";
        req.setContentLength(body.length());

        session.sendRequest(req) << body;

        Poco::Net::HTTPResponse response;
        session.receiveResponse(response);
    }

}

dpp::cluster& ClassroomBot::cluster() {
    return *m_Cluster;
}