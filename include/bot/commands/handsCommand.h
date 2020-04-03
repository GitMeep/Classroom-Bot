#pragma once

#include <bot/commands/command.h>
#include <list>
#include <mutex>

class HandsCommand : public Command {
public:
    void call(std::vector<std::string> parameters, aegis::gateway::events::message_create& message);
    CommandInfo getCommandInfo();

private:
    std::map<aegis::snowflake, std::list<aegis::user*>> _hands;
    std::mutex _handsMtx;

    std::string up(aegis::snowflake guildId, aegis::user& user);
    std::string down(aegis::snowflake guildId, aegis::user& user);
    std::string next(aegis::snowflake guildId);
    std::string clear(aegis::snowflake guildId);
    std::string list(aegis::snowflake guildId);
    std::string random(aegis::snowflake guildId);
    std::string pick(aegis::snowflake guildId, int number);
};