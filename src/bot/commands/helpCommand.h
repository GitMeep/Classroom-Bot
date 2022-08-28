#pragma once

#include <bot/commands/command.h>

class HelpCommand : public Command {
public:
    void call(int verb, const std::vector<std::string>& parameters, CommandContext* ctx);
    CommandInfo getCommandInfo();
};