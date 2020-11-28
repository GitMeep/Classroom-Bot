#include <cbpch.h>

#include <bot/commands/command.h>
#include <bot/bot.h>

Command::Command()
: m_Log(ClassroomBot::get().getLog())
, m_AegisCore(ClassroomBot::get().getAegis())
, m_Bot(&ClassroomBot::get()) {}