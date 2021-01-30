#pragma once

#include <bot/commands/command.h>

class InviteCommand : public Command {
public:
    void call(const std::vector<std::string>& parameters, CommandContext* ctx);
    CommandInfo getCommandInfo();
};