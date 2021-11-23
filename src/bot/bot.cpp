#include <spdlog/sinks/stdout_color_sinks.h>

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
#include <dpp/cluster.h>

enum ParserException {
    NOT_A_COMMAND,
    EMPTY_COMMAND
};

void ClassroomBot::init() {
    m_StartupTime = std::chrono::system_clock::now();

    m_Log = spdlog::stdout_color_mt("classroombot");
    m_Log->set_pattern("%^%Y-%m-%d %H:%M:%S.%e [%L] [ClassroomBot] [th#%t]%$ : %v");
    m_Log->set_level(spdlog::level::trace);

    m_Log->info("Starting ClassroomBot version " + std::string(BOT_VERSION));

    m_Config = std::make_shared<Config>();
    try {
        m_Config->loadFromFile("config.json");
    } catch (std::runtime_error& e) {
        m_Log->error(std::string(e.what()));
        return;
    }

    if(!m_Config->isLoaded()) {
        throw std::runtime_error("Invalid config supplied to ClassroomBot!");
    }

    std::string token = m_Config->get()["bot"]["token"];
    m_Cluster = std::make_shared<dpp::cluster>(
        token,
        dpp::intents::i_guilds |
        dpp::intents::i_guild_voice_states |
        dpp::intents::i_guild_messages |
        dpp::intents::i_guild_message_reactions |
        dpp::intents::i_direct_messages |
        dpp::intents::i_direct_message_reactions
    );

    m_Cluster->on_message_create(std::bind(&ClassroomBot::onMessage, this, std::placeholders::_1));

    m_Database = std::make_shared<DB>();
    m_SettingsRepo = std::make_shared<SettingsRepository>();
    m_QuestionRepo = std::make_shared<QuestionRepository>();
    m_HandRepo = std::make_shared<HandRepository>();
    m_MuteRepo = std::make_shared<MuteRepository>();
    m_Localization = std::make_shared<Localization>();
    m_CommandHandler = std::make_shared<CommandHandler>();

    m_Cluster->start_timer(dpp::timer_callback_t(std::bind(&ClassroomBot::updatePresence, this)), 60U);

    m_Initialized = true;
}

bool ClassroomBot::run() {
    if(!m_Initialized) return m_Initialized;

    m_Cluster->start(false);

    return true;
}

void ClassroomBot::registerCommand(Command* command) {
    m_CommandHandler->registerCommand(command);
}

void ClassroomBot::onMessage(const dpp::message_create_t& message) {
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
}

void ClassroomBot::updatePresence() {
    m_Log->info("Uptime: " + std::to_string(m_Cluster->uptime().hours) + " hours");

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

    m_HandRepo->expire();
    m_QuestionRepo->expire();

    if(guildCount == 0) return;

    if(m_Config->get()["topgg"]["enable"] == "true") {
        std::string topggToken = m_Config->get()["topgg"]["token"];
        std::string topggId = m_Config->get()["topgg"]["bot_id"];

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

ClassroomBot& ClassroomBot::getBot() {
    static ClassroomBot instance;
    return instance;
}

std::shared_ptr<Config> ClassroomBot::getConfig() {
    return getBot().m_Config;
}

std::shared_ptr<dpp::cluster> ClassroomBot::getCluster() {
    return getBot().m_Cluster;
}

std::shared_ptr<SettingsRepository> ClassroomBot::getSettingsRepo() {
    return getBot().m_SettingsRepo;
}

std::shared_ptr<QuestionRepository> ClassroomBot::getQuestionRepo() {
    return getBot().m_QuestionRepo;
}

std::shared_ptr<HandRepository> ClassroomBot::getHandRepo() {
    return getBot().m_HandRepo;
}

std::shared_ptr<MuteRepository> ClassroomBot::getMuteRepo() {
    return getBot().m_MuteRepo;
}

std::shared_ptr<DB> ClassroomBot::getDatabase() {
    return getBot().m_Database;
}

std::shared_ptr<spdlog::logger> ClassroomBot::getLog() {
    return getBot().m_Log;
}

std::shared_ptr<CommandHandler> ClassroomBot::getCommandHandler() {
    return getBot().m_CommandHandler;
}

std::shared_ptr<Localization> ClassroomBot::getLocalization() {
    return getBot().m_Localization;
}
