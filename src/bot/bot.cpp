#include "bot.h"
#include "commands/command.h"

#include <restclient-cpp/restclient.h>
#include <restclient-cpp/connection.h>

#include <string>
#include <chrono>
#include <thread>

#define COMMAND_PREFIX '?'

enum ParserException {
    NOT_A_COMMAND,
    EMPTY_COMMAND
};

ClassroomBot::ClassroomBot(std::string token, std::shared_ptr<spdlog::logger> log, std::shared_ptr<Config> config)
: _log(spdlog::get("aegis"))
,_aegisCore(std::make_shared<aegis::core>(aegis::create_bot_t().logger(log).token(token)))
,_config(config) {
    _aegisCore->wsdbg = true;
    _lastPresenceUpdate = std::chrono::system_clock::now()-std::chrono::seconds(61);

    _aegisCore->set_on_message_create(std::bind(&ClassroomBot::onMessage, this, std::placeholders::_1));
    _aegisCore->set_on_message_create_dm(std::bind(&ClassroomBot::onMessage, this, std::placeholders::_1));
    _aegisCore->set_on_guild_create(std::bind(&ClassroomBot::onGuildCreate, this, std::placeholders::_1));

    if(!_config->isLoaded()) {
        throw std::runtime_error("Invalid config supplied to ClassroomBot!");
    }

    _persistence = std::make_shared<Persistence>(_config);
    _settingsRepo = std::make_shared<SettingsRepo>(_persistence);
}

void ClassroomBot::run() {
    RestClient::init();
    _aegisCore->run();
    _aegisCore->yield();
    RestClient::disable();
}

void ClassroomBot::registerCommand(Command* command) {
    _commandHandler.registerCommand(command);
}

void ClassroomBot::onMessage(aegis::gateway::events::message_create message) {
    if(!message.has_user()) return;
    if(&message.get_user() == nullptr) {
        _log->debug("Nullptr");
        return;
    }
    if(message.get_user().is_bot()) return;
    if(message.msg.get_content().size() == 0) return;

    char prefix = _settingsRepo->getSettings(message.channel.get_guild_id()).prefix[0];
    if(message.msg.is_dm()) prefix = '?';

    if(_shouldUpdatePresence) {
        tryUpdatePresence();
    }

    std::string content = message.msg.get_content();
    if(content[0] != prefix) {
        return;
    }
    content = content.substr(1);

    bool success = _commandHandler.parseAndCall(content, {
        message.msg.get_id(),
        message.channel.get_id(),
        message.channel.get_guild_id(),
        message.get_user().get_id(),
        message.msg.is_dm()
    });

    if (!success) {
        message.channel.create_message("Unknown command");
    }
}

void ClassroomBot::onGuildCreate(aegis::gateway::events::guild_create obj) {
    _shouldUpdatePresence = !tryUpdatePresence(); // if presence couln't be updated, try to update again on next command, otherwise, don't try again
}

bool ClassroomBot::tryUpdatePresence() {
    std::chrono::system_clock::time_point now = std::chrono::system_clock::now();
    int secondsSinceLastUpdate = std::chrono::duration_cast<std::chrono::seconds>(now - _lastPresenceUpdate).count();
    if(secondsSinceLastUpdate > 60) { // only update presence if last update was more than 60 seconds ago
        updatePresence();
        _lastPresenceUpdate = std::chrono::system_clock::now();
        return true;
    } else {
        return false;
    }
}

void ClassroomBot::updatePresence() {
    int guildCount = _aegisCore->get_guild_count();
    _log->info("Updating presence: " + std::to_string(guildCount) + " servers");
    _aegisCore->update_presence(std::to_string(guildCount) + " servers", aegis::gateway::objects::activity::activity_type::Watching);

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
}