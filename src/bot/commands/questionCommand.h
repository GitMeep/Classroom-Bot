#pragma once

#include <bot/commands/command.h>

class QuestionCommand : public Command {
public:
    void call(const std::vector<std::string>& parameters, CommandContext* ctx);
    CommandInfo getCommandInfo();

private:
    void ask(CommandContext* ctx, std::string question);
    void list(CommandContext* ctx);
    void next(CommandContext* ctx);
    void clear(CommandContext* ctx);
};