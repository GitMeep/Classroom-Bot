#pragma once

#include <bot/commands/command.h>

class SettingsCommand : public Command {
public:
    void call(const std::vector<std::string>& parameters, CommandContext* ctx);
    CommandInfo getCommandInfo();

private:
    void set(CommandContext* ctx, const aegis::snowflake& guildId, const std::string& name, const std::string& value);
    void get(CommandContext* ctx, const aegis::snowflake& guildId);
};