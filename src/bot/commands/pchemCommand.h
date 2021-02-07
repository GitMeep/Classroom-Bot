#pragma once

#include <bot/commands/command.h>

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
    void call(int verb, const std::vector<std::string>& parameters, CommandContext* ctx);
    CommandInfo getCommandInfo();

private:
    std::string getCID(const std::string& query);
    PCResult getInfo(const std::string& cid);
};