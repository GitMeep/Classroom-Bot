#pragma once

#include <aegis.hpp>
#include <list>

#include "command.h"

class HandsCommand : public Command {
public:
    explicit HandsCommand(ClassroomBot* classroomBot) : Command::Command(classroomBot) {}
    void call(std::vector<std::string> parameters, CurrentCommand current);
    CommandInfo getCommandInfo();
    bool checkPermissions(aegis::permission channelPermissions);

private:
    std::map<aegis::snowflake, std::list<aegis::snowflake>> _hands;
    std::mutex _handsMtx;

    void up();
    void down();
    void next();
    void clear();
    void list();
    void random();
    void pick(int number);
};