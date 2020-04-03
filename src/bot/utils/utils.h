#pragma once

#include <aegis.hpp>

std::string getUsername(aegis::snowflake userId, aegis::snowflake guildId, std::shared_ptr<aegis::core> core);
bool isTeacher(aegis::snowflake guildId, aegis::snowflake userId, std::shared_ptr<aegis::core> core);