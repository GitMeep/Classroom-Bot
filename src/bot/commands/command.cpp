#include "command.h"

#include "../bot.h"

Command::Command(std::shared_ptr<ClassroomBot> classroomBot)
: _log(spdlog::get("aegis"))
, _aegisCore(classroomBot->_aegisCore)
, _bot(classroomBot) {}