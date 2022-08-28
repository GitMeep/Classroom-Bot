#pragma once

#include <bot/commands/command.h>

class AttendanceCommand : public Command {
public:
  AttendanceCommand();

  void command(const CommandContext& ctx);
  void buttonClick(const CommandContext& ctx);
};