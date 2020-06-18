#include "cbpch.h"

#include "command.h"
#include "../bot.h"

Command::Command()
: _log(spdlog::get("classroombot"))
, _aegisCore(ClassroomBot::get()._aegisCore)
, _bot(&ClassroomBot::get()) {}