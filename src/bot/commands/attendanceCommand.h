#pragma once

#include <bot/commands/command.h>

class AttendanceCommand : public Command {
public:
    AttendanceCommand();

    void command(const CommandContext& ctx);
    void userContext(const CommandContext& ctx);
    void messageContext(const CommandContext& ctx);
    void buttonClick(const CommandContext& ctx);
    void selectClick(const CommandContext& ctx);
    void formSubmit(const CommandContext& ctx);

private:

};