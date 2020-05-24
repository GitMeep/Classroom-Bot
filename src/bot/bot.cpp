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
    CommandInfo info = command->getCommandInfo();
    
    auto it = info.aliases.begin();
    std::string commandName = *it;

    _commands[commandName] = std::shared_ptr<Command>(command);

    while(it != info.aliases.end()) {
        _aliases[*it] = commandName;
        it++;
    }
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

    std::vector<std::string> parameters;
    try {
        std::string content = message.msg.get_content();
        parameters = parseCommand(content, prefix);
    } catch (ParserException& e) {
        switch(e) {
            case NOT_A_COMMAND:
                break;
            case EMPTY_COMMAND:
                break;
            default:
                _log->warn("Something went wrong in the parser!");
        }
        return;
    }

    std::string commandName = parameters[0];

    if(_shouldUpdatePresence) {
        tryUpdatePresence();
    }

    std::string ownerId = _config->getValue("bot")["owner"];
    if(commandName == "shutdown" && message.get_user().get_id().gets() == ownerId) {
        message.channel.create_message("Ok, shutting down.");
        _aegisCore->async([this]{
            std::this_thread::sleep_for(std::chrono::seconds(1));
            _aegisCore->shutdown();
        });
        return;
    }

    if(commandName == "he" || commandName == "help") {
        printHelp(message.get_user().get_id());
        return;
    }

    if(_aliases.count(commandName)) {
        commandName = _aliases[commandName];
    }

    if(!_commands.count(commandName)) {
        message.channel.create_message("Unknown command");
        return;
    }

    std::shared_ptr<Command> command = _commands[commandName];

    aegis::permission perms = message.channel.perms();
    if(!command->checkPermissions(perms)) {
        message.channel.create_message(command->getCommandInfo().permsMessage);
        return;
    }

    std::vector<std::string> commandParameters;
    auto it = parameters.begin();
    it++;
    while(it != parameters.end()) {
        commandParameters.push_back(*it);
        it++;
    }

    command->call(commandParameters, {message.msg.get_id(), message.channel.get_id(), message.channel.get_guild_id(), message.msg.get_author_id()});
    
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

void ClassroomBot::printHelp(aegis::snowflake userId) {
    std::stringstream ss;
    ss << "Please note that this bot is still in development. Please report any issues in the support server: https://discord.gg/dqmTAZY" << std::endl;
    ss << "Commands: ```" << std::endl
    << "help: Print this help page." << std::endl;
    ss << "Aliases: " << std::endl
    << "\the" << std::endl
    << "------------------------------------------------" << std::endl << std::endl;
    
    CommandInfo commandInfo;
    auto command = _commands.begin();
    while(command != _commands.end()) {
        commandInfo = (command->second)->getCommandInfo();
        ss << command->first << ": " << commandInfo.description << std::endl;
        
        if(commandInfo.aliases.size() > 1)
            ss << "Aliases:" << std::endl;
        auto alias = commandInfo.aliases.begin();
        alias++;
        while(alias != commandInfo.aliases.end()) {
            ss << "\t" << *alias << std::endl;
            alias++;
        }

        if(commandInfo.options.size())
            ss << "Options: " << std::endl;
        auto option = commandInfo.options.begin();
        while(option != commandInfo.options.end()) {
            ss << "\t" << *option << std::endl;
            option++;
        }

        if(command != (_commands.end()--))
            ss << std::endl << "------------------------------------------------" << std::endl << std::endl;

        command++;
    }

    ss << "```";

    _aegisCore->create_dm_message(userId, ss.str());
}

std::vector<std::string> ClassroomBot::parseCommand(std::string& message, char prefix) {
    std::vector<std::string> parameters;
    std::stringstream ss;
    ss << message;

    std::string current;
    ss >> current;

    if(current.at(0) != prefix) {
        throw ParserException::NOT_A_COMMAND;
    }

    current = current.substr(1, current.length()-1);

    if(current.length() == 0) throw ParserException::EMPTY_COMMAND; // someone sent a message which was just a questionmark

    std::transform(current.begin(), current.end(), current.begin(), [](unsigned char c){ return std::tolower(c); });

    parameters.emplace_back(current);

    while(ss >> current) {
        parameters.emplace_back(current);
    }

    return parameters;

}