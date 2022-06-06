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
    void up(const CommandContext& ctx);
    void down(const CommandContext& ctx);
    void next(const CommandContext& ctx);
    void clear(const CommandContext& ctx);
    void random(const CommandContext& ctx);
    void pickList(const CommandContext& ctx);
    void pick(const CommandContext& ctx);
};