#include <bot/bot.h>
#include <bot/commands/attendanceCommand.h>
#include <bot/commands/handsCommand.h>
#include <bot/commands/inviteCommand.h>
#include <bot/commands/manage.h>
#include <bot/commands/questionCommand.h>
#include <bot/commands/settingsCommand.h>
#include <bot/config/config.h>
//#include <bot/commands/helpCommand.h>
//#include <bot/commands/muteCommand.h>

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <iostream>

int main() {
  ClassroomBot::init();

  ClassroomBot::registerManageCommand(new ManageCommand());

  ClassroomBot::registerCommand(new HandsCommand());
  ClassroomBot::registerCommand(new AttendanceCommand());
  ClassroomBot::registerCommand(new InviteCommand());
  ClassroomBot::registerCommand(new QuestionCommand());
  /*
  ClassroomBot::registerCommand(new HelpCommand());
  ClassroomBot::registerCommand(new MuteCommand());
  */
  ClassroomBot::registerCommand(new SettingsCommand());
  ClassroomBot::run();
  return 0;
}