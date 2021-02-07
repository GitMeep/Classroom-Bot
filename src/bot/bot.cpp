#include <cbpch.h>

#include <bot/commands/command.h>
#include <bot/config/config.h>
#include <bot/persistence/db.h>
#include <bot/persistence/repo/settingsRepo.h>
#include <bot/persistence/repo/handRepo.h>
#include <bot/persistence/repo/questionRepo.h>
#include <bot/persistence/repo/muteRepo.h>
#include <bot/localization/localization.h>

#include <bot/bot.h>

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
    m_AegisCore = std::make_shared<aegis::core>(aegis::create_bot_t()
        .token(token)
    #ifdef DEBUG
        .log_level(spdlog::level::trace)
    #else
        .log_level(spdlog::level::warn)
    #endif
        .log_format("%^%Y-%m-%d %H:%M:%S.%e [%L] [Aegis] [th#%t]%$ : %v")
        .intents(
            aegis::intent::Guilds |
            aegis::intent::GuildMembers |
            aegis::intent::GuildVoiceStates |
            aegis::intent::GuildMessages |
            aegis::intent::GuildMessageReactions |
            aegis::intent::DirectMessages |
            aegis::intent::DirectMessageReactions
        ));

    m_AegisCore->set_on_message_create(std::bind(&ClassroomBot::onMessage, this, std::placeholders::_1));
    m_AegisCore->set_on_message_create_dm(std::bind(&ClassroomBot::onMessage, this, std::placeholders::_1));

    m_AegisCore->update_presence("Starting, please wait", aegis::gateway::objects::activity::activity_type::Game);

    m_Database = std::make_shared<DB>();
    m_SettingsRepo = std::make_shared<SettingsRepository>();
    m_QuestionRepo = std::make_shared<QuestionRepository>();
    m_HandRepo = std::make_shared<HandRepository>();
    m_MuteRepo = std::make_shared<MuteRepository>();
    m_Localization = std::make_shared<Localization>();

    m_CommandHandler = std::make_shared<CommandHandler>();

    m_PresenceTimer = std::make_unique<asio::steady_timer>(m_AegisCore->get_io_context(), asio::chrono::minutes(1));
    asio::post(m_AegisCore->get_io_context(), [this] {
        updatePresence();
    });

    m_Initialized = true;
}

ClassroomBot& ClassroomBot::get() {
    static ClassroomBot instance;
    return instance;
}

bool ClassroomBot::run() {
    if(!m_Initialized) return m_Initialized;

    RestClient::init();
    m_AegisCore->run();
    m_AegisCore->yield();
    RestClient::disable();

    return true;
}

void ClassroomBot::registerCommand(Command* command) {
    m_CommandHandler->registerCommand(command);
}

void ClassroomBot::onMessage(aegis::gateway::events::message_create message) {
    if(!message.has_user()) return;
    if(&message.get_user() == nullptr) return;
    if(message.msg.get_content().size() == 0) return;

    CommandContext ctx(
        message.msg.get_id(),
        message.channel.get_id(),
        message.channel.get_guild_id(),
        message.get_user().get_id(),
        message.msg.is_dm(),
        m_SettingsRepo->get(message.channel.get_guild_id())
    );

    std::string prefix = ctx.getSettings().prefix;
    if(ctx.isDM()) prefix = "?";

    std::string content = message.msg.get_content();
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
    int guildCount = m_AegisCore->get_guild_count();
    auto now = std::chrono::system_clock::now();
    auto uptime = std::chrono::duration_cast<std::chrono::hours>(now - m_StartupTime);

    m_Log->info("Uptime: " + std::to_string(uptime.count()) + " hours");

    switch (m_PresenceState) {
        case 0:
            m_AegisCore->update_presence("?help", aegis::gateway::objects::activity::activity_type::Watching);
            break;
        
        case 1:
            m_AegisCore->update_presence(std::to_string(guildCount) + " servers", aegis::gateway::objects::activity::activity_type::Watching);
            break;

        default:
            break;
    }

    m_PresenceState++;
    m_PresenceState = m_PresenceState % 2;

    if(m_Config->get()["topgg"]["enable"] == "true") {
        std::string topggToken = m_Config->get()["topgg"]["token"];
        std::string topggId = m_Config->get()["topgg"]["bot_id"];
        RestClient::Connection* conn = new RestClient::Connection("https://top.gg");

        RestClient::HeaderFields headers;
        headers["Authorization"] = topggToken;
        headers["Content-Type"] = "application/json";
        conn->SetHeaders(headers);

        std::string data = "{\"server_count\": " + std::to_string(guildCount) + "}";
        auto r = conn->post("/api/bots/" + topggId + "/stats", data);

        delete conn;
    }

    m_HandRepo->expire();
    m_QuestionRepo->expire();

    m_PresenceTimer->expires_after(asio::chrono::minutes(1));
    m_PresenceTimer->async_wait(std::bind(&ClassroomBot::updatePresence, this));

}


std::shared_ptr<Config> ClassroomBot::getConfig() {
    return this->m_Config;
}

std::shared_ptr<aegis::core> ClassroomBot::getAegis() {
    return this->m_AegisCore;
}

std::shared_ptr<SettingsRepository> ClassroomBot::getSettingsRepo() {
    return this->m_SettingsRepo;
}

std::shared_ptr<QuestionRepository> ClassroomBot::getQuestionRepo() {
    return this->m_QuestionRepo;
}

std::shared_ptr<HandRepository> ClassroomBot::getHandRepo() {
    return this->m_HandRepo;
}

std::shared_ptr<MuteRepository> ClassroomBot::getMuteRepo() {
    return this->m_MuteRepo;
}

std::shared_ptr<DB> ClassroomBot::getDatabase() {
    return this->m_Database;
}

std::shared_ptr<spdlog::logger> ClassroomBot::getLog() {
    return this->m_Log;
}

std::shared_ptr<CommandHandler> ClassroomBot::getCommandHandler() {
    return this->m_CommandHandler;
}

std::shared_ptr<Localization> ClassroomBot::getLocalization() {
    return this->m_Localization;
}
