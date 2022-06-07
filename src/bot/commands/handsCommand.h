#pragma once

#include <bot/commands/command.h>

class HandsCommand : public Command {
public:
    HandsCommand();

    void command(const CommandContext& ctx);
    void userContext(const CommandContext& ctx);
    void messageContext(const CommandContext& ctx);
    void buttonClick(const CommandContext& ctx);
    void selectClick(const CommandContext& ctx);
    void formSubmit(const CommandContext& ctx);

private:
    // slash command
    void up(const CommandContext& ctx);
    void down(const CommandContext& ctx);
    void next(const CommandContext& ctx);
    void clear(const CommandContext& ctx);
    void random(const CommandContext& ctx);
    void list(const CommandContext& ctx);
    void pickCommand(const CommandContext& ctx);

    // select menu
    void pickFromList(const CommandContext& ctx);

    // user context
    void pickUserContext(const CommandContext& ctx);
};