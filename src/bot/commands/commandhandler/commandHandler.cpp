#include "commandHandler.h"

bool CommandHandler::parseAndCall(std::string input, MessageInfo messageInfo) {
    std::vector<std::string> parameters;
    std::stringstream ss;
    ss << input;

    std::string commandName;
    ss >> commandName;

    std::string param;
    while(ss >> param) {
        parameters.emplace_back(param);
    }

    return callCommand(commandName, parameters, messageInfo);
}

bool CommandHandler::callCommand(std::string name, std::vector<std::string> parameters, MessageInfo messageInfo) {
    std::transform(name.begin(), name.end(), name.begin(), [](unsigned char c){ return std::tolower(c); }); // make sure name is lowercase

    std::string commandName = name;
    if (_aliases.count(name)) {
        commandName = _aliases[name];
    }

    if(!_commands.count(commandName)) return false;

    _commands[commandName]->call(parameters, messageInfo);
    return true;
}

void CommandHandler::registerCommand(Command* command) {
    CommandInfo info = command->getCommandInfo();

    if (_commands.count(info.name)) return; // command already exists
    if (_aliases.count(info.name)) return; // command already exists

    _commands[info.name] = std::shared_ptr<Command>(command);
    _commandInfos[info.name] = info;

    auto alias = info.aliases.begin();
    while (alias != info.aliases.end()) {
        _aliases[*alias] = info.name;
        alias++;
    }
    
}

std::vector<CommandInfo> CommandHandler::getInfo(std::string commandName) {
    std::vector<CommandInfo> info;
    if(commandName == "") {
        auto help = _commandInfos.begin();
        while(help != _commandInfos.end()) {
            info.emplace_back(help->second);
            help++;
        }
        return info;
    }

    std::string name = commandName;
    if (_aliases.count(name)) {
        name = _aliases[commandName];
    }

    if(_commandInfos.count(name)) {
        info.emplace_back(_commandInfos[name]);
        return info;
    }

    return info;
}