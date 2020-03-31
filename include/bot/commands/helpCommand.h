#pragma once

#include <bot/commands/command.h>

class HelpCommand : public Command {
public:
    void call(std::vector<std::string> parameters, aegis::gateway::events::message_create& message);
    CommandInfo getCommandInfo();
};