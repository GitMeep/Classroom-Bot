#pragma once

#include <aegis.hpp>
#include <list>

#include "command.h"

class InviteCommand : public Command {
public:
    explicit InviteCommand(std::shared_ptr<ClassroomBot> classroomBot) : Command::Command(classroomBot) {}
    void call(std::vector<std::string> parameters, CurrentCommand current);
    CommandInfo getCommandInfo();
    bool checkPermissions(aegis::permission channelPermissions);
};