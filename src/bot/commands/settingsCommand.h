#pragma once

#include <bot/commands/command.h>

class SettingsCommand : public Command {
public:
    SettingsCommand();

    void command(const CommandContext& ctx);
    void selectClick(const CommandContext& ctx);

private:
    void language(const CommandContext& ctx);
};
