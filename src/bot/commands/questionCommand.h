#pragma once

#include <aegis.hpp>

#include "command.h"

class QuestionCommand : public Command {
public:
    QuestionCommand(std::shared_ptr<aegis::core> aegisCore) : Command::Command(aegisCore->log, aegisCore) {}
    void call(std::vector<std::string> parameters, CurrentCommand current);
    CommandInfo getCommandInfo();

private:
    struct Question {
        aegis::snowflake user;
        std::string question;
    };

    std::map<aegis::snowflake, std::deque<Question>> _questions;
    std::mutex _questionsMtx;

    void ask(std::string question);
    void list();
    void next();
    void clear();
};