#pragma once

#include <aegis.hpp>
#include <list>

#include "command.h"

struct PCResult {
    std::string name;
    std::string cid;
    std::string formula;
    std::string structUrl;
    std::string molMass;
    std::string ghsReference;
    std::vector<std::string> warningLabels;
    std::vector<std::string> ghsMessages;
    std::string precautions;
};

class PchemCommand : public Command {
public:
    void call(const std::vector<std::string>& parameters, MessageInfo* current);
    CommandInfo getCommandInfo();

private:
    std::string getCID(std::string query);
    PCResult getInfo(std::string cid);
};