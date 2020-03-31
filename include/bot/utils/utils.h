#pragma once

#include <aegis.hpp>

std::string getUsername(aegis::user &user, aegis::snowflake guildId);
bool isTeacher(aegis::gateway::events::message_create message);