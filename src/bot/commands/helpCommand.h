#pragma once

#include <aegis.hpp>
#include <list>

#include "command.h"

class HelpCommand : public Command {
public:
    explicit HelpCommand(std::shared_ptr<ClassroomBot> classroomBot) : Command::Command(classroomBot) {}
    void call(std::vector<std::string> parameters, MessageInfo current);
    CommandInfo getCommandInfo();
    bool checkPermissions(aegis::permission channelPermissions);
};