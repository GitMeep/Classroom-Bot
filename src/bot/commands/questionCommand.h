#pragma once

#include <bot/commands/command.h>

class QuestionCommand : public Command {
public:
    void call(int verb, const std::vector<std::string>& parameters, CommandContext* ctx);
    CommandInfo getCommandInfo();

private:
    void ask(CommandContext* ctx, const std::vector<std::string>& parameters);
    void list(CommandContext* ctx);
    void next(CommandContext* ctx);
    void clear(CommandContext* ctx);
};