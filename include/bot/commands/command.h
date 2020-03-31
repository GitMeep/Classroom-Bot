#pragma once

#include <vector>
#include <string>
#include <aegis.hpp>

struct CommandInfo {
    std::vector<std::string> aliases;
};

class Command {
public:
    virtual void call(std::vector<std::string> parameters, aegis::gateway::events::message_create& message) = 0; // call the command
    virtual CommandInfo getCommandInfo() = 0; // called on command registration

    virtual ~Command() {}
};