#include <iostream>

//#include <bot/commands/command.h>
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
    std::cout << "Starting ClassroomBot version " << std::string(BOT_VERSION) << std::endl;

    m_Cluster->on_slashcommand(ClassroomBot::onCommand);

    m_Cluster->on_ready([](const dpp::ready_t& event) {
        if (dpp::run_once<struct register_bot_commands>()) {
            #ifdef DEBUG
            ClassroomBot::cluster().guild_command_create(
                dpp::slashcommand("ping", "Ping pong!", ClassroomBot::cluster().me.id),
                705355899400880212
            );
            #else
            ClassroomBot::cluster()->global_command_create(
                dpp::slashcommand("ping", "Ping pong!", ClassroomBot::cluster().me.id)
            );
            #endif
            LOG_INFO("Registered slash commands");
        }
    });

    cluster().start_timer(ClassroomBot::updatePresence, 60U);

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
    // todo
}

void ClassroomBot::onCommand(const dpp::slashcommand_t& event) {
    if (event.command.get_command_name() == "ping") {
        event.reply("Pong!");
    }
    /*
    if(message.msg->content.size() == 0) return;

    CommandContext ctx(
        message.msg->id,
        message.msg->channel_id,
        message.msg->guild_id,
        message.msg->member.user_id,
        dpp::find_channel(message.msg->channel_id)->is_dm(),
        m_SettingsRepo->get(message.msg->guild_id)
    );

    std::string prefix = ctx.getSettings().prefix;
    if(ctx.isDM()) prefix = "?";

    std::string content = message.msg->content;
    bool isHelp = content.substr(0, 5) == "?help"; // if someone types ?help, ignore the actual prefix (1), and set the prefix to "?" (2), parsing it as a normal command. This ensures that ?help always works
    if(content.substr(0, prefix.length()) != prefix && !isHelp) {
        return;
    }
    if(isHelp) prefix = "?";

    content = content.substr(prefix.length());
    bool success = m_CommandHandler->parseAndCall(content, &ctx);

    if (!success) {
        ctx.respond("unknown_cmd");
        return;
    }
    */
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