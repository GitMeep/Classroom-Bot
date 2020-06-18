#pragma once

#include "command.h"

class HandsCommand : public Command {
public:
    explicit HandsCommand() : Command::Command() {}
    void call(const std::vector<std::string>& parameters, MessageInfo* current);
    CommandInfo getCommandInfo();

private:
    void up(MessageInfo* current);
    void down(MessageInfo* current);
    void next(MessageInfo* current);
    void clear(MessageInfo* current);
    void list(MessageInfo* current);
    void random(MessageInfo* current);
    void pick(MessageInfo* current, int number);
};