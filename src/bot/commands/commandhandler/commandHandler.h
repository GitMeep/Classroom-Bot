#pragma once

#include <vector>
#include <map>

#include "../command.h"

class CommandHandler {
public:
    bool parseAndCall(std::string input, MessageInfo messageInfo);
    bool callCommand(std::string name, std::vector<std::string> parameters, MessageInfo messageInfo);
    void registerCommand(Command* command);
    std::vector<CommandInfo> getInfo(std::string commandName);

private:
    std::unordered_map<std::string, std::shared_ptr<Command>> _commands; // command name -> command class
    std::unordered_map<std::string, std::string> _aliases; // alias -> command name
    std::unordered_map<std::string, CommandInfo> _commandInfos; // command name -> info
};