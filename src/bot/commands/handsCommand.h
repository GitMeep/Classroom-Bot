#pragma once

#include <bot/commands/command.h>

class HandsCommand : public Command {
public:
    explicit HandsCommand() : Command::Command() {}
    void call(const std::vector<std::string>& parameters, CommandContext* ctx);
    CommandInfo getCommandInfo();

private:
    void up(CommandContext* ctx);
    void down(CommandContext* ctx);
    void next(CommandContext* ctx);
    void clear(CommandContext* ctx);
    void list(CommandContext* ctx);
    void random(CommandContext* ctx);
    void pick(CommandContext* ctx, int number);
};