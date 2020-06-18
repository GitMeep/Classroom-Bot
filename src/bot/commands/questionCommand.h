#pragma once

#include <aegis.hpp>

#include "command.h"

class QuestionCommand : public Command {
public:
    void call(const std::vector<std::string>& parameters, MessageInfo* current);
    CommandInfo getCommandInfo();

private:
    void ask(MessageInfo* current, std::string question);
    void list(MessageInfo* current);
    void next(MessageInfo* current);
    void clear(MessageInfo* current);
};