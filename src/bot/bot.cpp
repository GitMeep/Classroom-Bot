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
    
    for(auto alias : info.aliases) {
        _commands[alias] = std::shared_ptr<Command>(command);
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

    if(!_commands.count(parameters[0])) {
        message.channel.create_message("Unknown command");
    }

    std::vector<std::string> commandParameters;
    for(auto it = parameters.begin(); it != parameters.end(); ++it) {
        if(it == parameters.begin()) continue;
        commandParameters.push_back(*it);
    }

    std::shared_ptr<Command> command = _commands[parameters[0]];
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