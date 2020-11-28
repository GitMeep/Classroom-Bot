#pragma once

#include <bot/commands/command.h>

class SettingsCommand : public Command {
public:
    void call(const std::vector<std::string>& parameters, MessageInfo* current);
    CommandInfo getCommandInfo();

private:
    void set(MessageInfo* current, const aegis::snowflake& guildId, const std::string& name, const std::string& value);
    void get(MessageInfo* current, const aegis::snowflake& guildId);
};