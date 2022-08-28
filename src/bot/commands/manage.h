#pragma once

#include <bot/commands/command.h>

#define MANAGEMENT_SERVER      1013047644257525801
#define ERROR_CRITICAL_CHANNEL 1013059144053424178

// /manage restart // restart bot
// /manage uptime // print uptime
// /manage guilds // print amount of guilds that bot is int
// /manage task [name] // run a task

class ManageCommand : public Command {
public:
  ManageCommand();

  void command(const CommandContext& ctx);

private:
  // slash commands
  void restartCmd(const CommandContext& ctx);
  void uptimeCmd(const CommandContext& ctx);
  void guildsCmd(const CommandContext& ctx);
  void taskCmd(const CommandContext& ctx);
};