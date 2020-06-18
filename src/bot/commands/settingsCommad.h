#pragma once

#include <aegis.hpp>
#include <list>

#include "command.h"

class SettingsCommand : public Command {
public:
    void call(const std::vector<std::string>& parameters, MessageInfo* current);
    CommandInfo getCommandInfo();

private:
    void set(MessageInfo* current, aegis::snowflake guildId, std::string name, std::string value);
    void get(MessageInfo* current, aegis::snowflake guildId);
    std::mutex _mtx;
};