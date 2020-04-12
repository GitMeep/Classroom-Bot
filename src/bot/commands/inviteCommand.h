#pragma once

#include <aegis.hpp>
#include <list>

#include "command.h"

class InviteCommand : public Command {
public:
    InviteCommand(std::shared_ptr<aegis::core> aegisCore) : Command::Command(aegisCore->log, aegisCore) {}
    void call(std::vector<std::string> parameters, CurrentCommand current);
    CommandInfo getCommandInfo();
};