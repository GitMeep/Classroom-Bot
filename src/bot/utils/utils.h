#pragma once

#include <aegis.hpp>
#include "../persistence/settings.h"

std::string getUsername(aegis::snowflake userId, aegis::snowflake guildId, std::shared_ptr<aegis::core> core);
bool isTeacher(aegis::snowflake guildId, aegis::snowflake userId, std::shared_ptr<aegis::core> core, std::shared_ptr<SettingsRepo> settings);