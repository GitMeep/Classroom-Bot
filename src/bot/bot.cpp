#include "bot/bot.h"
#include <fmt/format.h>
#include <bot/commands/command.h>

#define COMMAND_PREFIX '?'

enum ParserException {
    NOT_A_COMMAND,
    EMPTY_COMMAND
};

questionsbot::questionsbot(std::shared_ptr<spdlog::logger> logger) {
    _log = logger;
}

void questionsbot::registerCommand(Command* command) {
    CommandInfo info = command->getCommandInfo();
    
    auto it = info.aliases.begin();
    std::string commandName = *it;

    _commands[commandName] = std::shared_ptr<Command>(command);

    while(it != info.aliases.end()) {
        _aliases[*it] = commandName;
        it++;
    }
}

void questionsbot::onMessage(aegis::gateway::events::message_create message) {
    if(message.get_user().is_bot()) return;
    if(message.msg.get_content().size() == 0) return;
    std::vector<std::string> parameters;
    try {
        parameters = parseCommand(message.msg.get_content(), COMMAND_PREFIX);
    } catch (ParserException e) {
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
    if(_aliases.count(commandName)) {
        commandName = _aliases[commandName];
    }

    if(!_commands.count(commandName)) {
        message.channel.create_message("Unknown command");
        return;
    }

    std::vector<std::string> commandParameters;
    auto it = parameters.begin();
    it++;
    while(it != parameters.end()) {
        commandParameters.push_back(*it);
        it++;
    }

    std::shared_ptr<Command> command = _commands[commandName];
    command->call(commandParameters, message);
    
}

std::vector<std::string> questionsbot::parseCommand(std::string message, char prefix) {
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