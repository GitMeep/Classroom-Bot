#pragma once

#include <aegis.hpp>
#include <list>

#include "command.h"

class SettingsCommand : public Command {
public:
    explicit SettingsCommand(std::shared_ptr<ClassroomBot> classroomBot) : Command::Command(classroomBot) {}
    void call(std::vector<std::string> parameters, CurrentCommand current);
    CommandInfo getCommandInfo();
    bool checkPermissions(aegis::permission channelPermissions);

private:
    void set(aegis::snowflake guildId, std::string name, std::string value);
    void get(aegis::snowflake guildId);
    std::mutex _mtx;
};