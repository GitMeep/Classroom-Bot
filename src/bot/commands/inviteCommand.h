#pragma once

#include <bot/commands/command.h>

class InviteCommand : public Command {
public:
    InviteCommand();

    void command(const CommandContext& ctx);
};
