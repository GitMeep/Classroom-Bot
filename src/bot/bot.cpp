#include "bot.h"
#include "commands/command.h"

#define COMMAND_PREFIX '?'

enum ParserException {
    NOT_A_COMMAND,
    EMPTY_COMMAND
};

ClassroomBot::ClassroomBot(std::shared_ptr<spdlog::logger> logger, aegis::core* core)
    : _log(logger)
    ,_aegisCore(core) {}

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

    if(commandName == "h" || commandName == "help") {
        printHelp(message.channel.get_id());
        return;
    }

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
    command->call(commandParameters, {message.msg.get_id(), message.channel.get_id(), message.channel.get_guild_id(), message.msg.get_author_id()});
    
}

void ClassroomBot::printHelp(aegis::snowflake channelId) {
    std::stringstream ss;
    ss << "Commands: ```" << std::endl
    << "help: Print this help page." << std::endl
    << "Aliases: " << std::endl
    << "\th" << std::endl
    << "------------------------------------------------" << std::endl << std::endl;
    
    CommandInfo commandInfo;
    auto command = _commands.begin();
    while(command != _commands.end()) {
        commandInfo = (command->second)->getCommandInfo();
        ss << command->first << ": " << commandInfo.description << std::endl;
        
        if(commandInfo.aliases.size())
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
            ss << "------------------------------------------------" << std::endl << std::endl;

        command++;
    }

    ss << "```";

    _aegisCore->create_message(channelId, ss.str());
}

std::vector<std::string> ClassroomBot::parseCommand(std::string message, char prefix) {
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