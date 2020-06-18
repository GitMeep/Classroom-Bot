#include "cbpch.h"

#include "commands/command.h"
#include "config/config.h"
#include "bot/persistence/db.h"
#include "persistence/repo/settingsRepo.h"
#include "persistence/repo/handRepo.h"
#include "persistence/repo/questionRepo.h"
#include "persistence/repo/muteRepo.h"

#include "bot.h"

enum ParserException {
    NOT_A_COMMAND,
    EMPTY_COMMAND
};

void ClassroomBot::init(const std::string& token, const std::shared_ptr<Config>& config) {
    _log = spdlog::get("classroombot");
    _log->info("Starting ClassroomBot version " + std::string(BOT_VERSION));

    _aegisCore = std::make_shared<aegis::core>(aegis::create_bot_t().token(token));
    _config = config;
    _aegisCore->wsdbg = true;

    _aegisCore->set_on_message_create(std::bind(&ClassroomBot::onMessage, this, std::placeholders::_1));
    _aegisCore->set_on_message_create_dm(std::bind(&ClassroomBot::onMessage, this, std::placeholders::_1));

    if(!_config->isLoaded()) {
        throw std::runtime_error("Invalid config supplied to ClassroomBot!");
    }

    nlohmann::json persistence = _config->getValue("persistence");
    bool persistenceEnabled = persistence["enable"] == "true";
    std::string connString = persistence["url"];

    _database = std::make_shared<DB>(connString);

    _settingsRepo = std::make_shared<SettingsRepository>(_database);
    _questionRepo = std::make_shared<QuestionRepository>(_database);
    _handRepo = std::make_shared<HandRepository>(_database);
    m_MuteRepo = std::make_shared<MuteRepository>(_database);

    m_PresenceTimer = std::make_unique<asio::steady_timer>(_aegisCore->get_io_context(), asio::chrono::minutes(1));
    asio::post(_aegisCore->get_io_context(), [this] {
        //m_PresenceTimer->async_wait(std::bind(&ClassroomBot::updatePresence, this));
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
    _aegisCore->run();
    _aegisCore->yield();
    RestClient::disable();

    return true;
}

void ClassroomBot::registerCommand(Command* command) {
    _commandHandler.registerCommand(command);
}

void ClassroomBot::onMessage(aegis::gateway::events::message_create message) {
    if(!message.has_user()) return;
    if(&message.get_user() == nullptr) return;
    if(message.get_user().is_bot()) return;
    if(message.msg.get_content().size() == 0) return;

    std::string prefix = _settingsRepo->get(message.channel.get_guild_id()).prefix;
    if(message.msg.is_dm()) prefix = "?";

    std::string content = message.msg.get_content();
    bool isHelp = content.substr(0, 5) == "?help"; // if someone types ?help, ignore the actual prefix (1), and set the prefix to "?" (2), parsing it as a normal command. This ensures that ?help always works
    if(content.substr(0, prefix.length()) != prefix && !isHelp) {
        return;
    }
    if(isHelp) prefix = "?";
    content = content.substr(prefix.length());

    MessageInfo info = {
        message.msg.get_id(),
        message.channel.get_id(),
        message.channel.get_guild_id(),
        message.get_user().get_id(),
        message.msg.is_dm()
    };
    bool success = _commandHandler.parseAndCall(content, &info);

    if (!success) {
        message.channel.create_message("Unknown command");
    }
}

void ClassroomBot::updatePresence() {
    int guildCount = _aegisCore->get_guild_count();
    switch (m_PresenceState) {
        case 0:
            _aegisCore->update_presence("?help", aegis::gateway::objects::activity::activity_type::Watching);
            break;
        
        default:
            _aegisCore->update_presence(std::to_string(guildCount) + " servers", aegis::gateway::objects::activity::activity_type::Watching);
            break;
    }

    m_PresenceState++;
    m_PresenceState = m_PresenceState % 2;

    if(_config->getValue("topgg")["enable"] == "true") {
        std::string topggToken = _config->getValue("topgg")["token"];
        std::string topggId = _config->getValue("topgg")["bot_id"];
        RestClient::Connection* conn = new RestClient::Connection("https://top.gg");

        RestClient::HeaderFields headers;
        headers["Authorization"] = topggToken;
        headers["Content-Type"] = "application/json";
        conn->SetHeaders(headers);

        std::string data = "{\"server_count\": " + std::to_string(guildCount) + "}";
        auto r = conn->post("/api/bots/" + topggId + "/stats", data);
    }

    m_PresenceTimer->expires_after(asio::chrono::minutes(1));
    m_PresenceTimer->async_wait(std::bind(&ClassroomBot::updatePresence, this));
}
