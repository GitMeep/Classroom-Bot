#pragma once

#include <aegis.hpp>

#include "command.h"

class QuestionCommand : public Command {
public:
    explicit QuestionCommand(ClassroomBot* classroomBot) : Command::Command(classroomBot) {}
    void call(std::vector<std::string> parameters, CurrentCommand current);
    CommandInfo getCommandInfo();
    bool checkPermissions(aegis::permission channelPermissions);

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