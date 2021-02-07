#pragma once

#include <bot/commands/command.h>

class SettingsCommand : public Command {
public:
    void call(int verb, const std::vector<std::string>& parameters, CommandContext* ctx);
    CommandInfo getCommandInfo();

private:
    void set(CommandContext* ctx, const std::vector<std::string>& parameters);
    void get(CommandContext* ctx);
};