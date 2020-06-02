#pragma once

#include <aegis.hpp>
#include <list>

#include "command.h"

struct PCResult {
    std::string name;
    std::string cid;
    std::string structUrl;
    std::string molMass;
    std::string ghsReference;
    std::vector<std::string> warningLabels;
    std::vector<std::string> ghsMessages;
    std::string precautions;
};

class PchemCommand : public Command {
public:
    explicit PchemCommand(std::shared_ptr<ClassroomBot> classroomBot) : Command::Command(classroomBot) {}
    void call(std::vector<std::string> parameters, MessageInfo current);
    CommandInfo getCommandInfo();
    bool checkPermissions(aegis::permission channelPermissions);

private:
    std::string getCID(std::string query);
    PCResult getInfo(std::string cid);
};