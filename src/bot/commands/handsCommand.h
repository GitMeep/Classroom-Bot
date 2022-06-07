#pragma once

#include <bot/commands/command.h>

class HandsCommand : public Command {
public:
    HandsCommand();

    void command(const CommandContext& ctx);
    void userContext(const CommandContext& ctx);
    void selectClick(const CommandContext& ctx);

private:
    // slash commands
    void handCmd(const CommandContext& ctx);
    void raiseCmd(const CommandContext& ctx);
    void lowerCmd(const CommandContext& ctx);
    void pickCmd(const CommandContext& ctx);

    // hand subcommands
    void handNext(const CommandContext& ctx);
    void handClear(const CommandContext& ctx);
    void handRandom(const CommandContext& ctx);
    void handList(const CommandContext& ctx);

    // select menu
    void pickFromList(const CommandContext& ctx);

    // user context
    void pickUserContext(const CommandContext& ctx);
};