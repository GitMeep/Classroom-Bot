#pragma once

#include "command.h"

class InviteCommand : public Command {
public:
    void call(const std::vector<std::string>& parameters, MessageInfo* current);
    CommandInfo getCommandInfo();
};