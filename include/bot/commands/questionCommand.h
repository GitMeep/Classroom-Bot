#pragma once

#include <bot/commands/command.h>

class QuestionCommand : public Command {
public:
    void call(std::vector<std::string> parameters, aegis::gateway::events::message_create& message);
    CommandInfo getCommandInfo();

private:
    struct Question {
        std::reference_wrapper<aegis::user> user;
        std::string question;
    };

    std::map<aegis::snowflake, std::deque<Question>> _questions;
    void ask(std::string message, aegis::snowflake guildId, std::reference_wrapper<aegis::user> user);
    std::string list(aegis::snowflake guildId);
};