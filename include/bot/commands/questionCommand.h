#pragma once

#include <bot/commands/command.h>
#include <mutex>

class QuestionCommand : public Command {
public:
    QuestionCommand();
    void call(std::vector<std::string> parameters, aegis::gateway::events::message_create& message);
    CommandInfo getCommandInfo();

private:
    struct Question {
        std::reference_wrapper<aegis::user> user;
        std::string question;
    };

    std::shared_ptr<spdlog::logger> _log;
    std::map<aegis::snowflake, std::deque<Question>> _questions;
    std::mutex _questionsMtx;

    std::string ask(std::string message, aegis::snowflake guildId, std::reference_wrapper<aegis::user> user);
    std::string list(aegis::snowflake guildId);
    std::string next(aegis::snowflake guildId);
    std::string clear(aegis::snowflake guildId);
};