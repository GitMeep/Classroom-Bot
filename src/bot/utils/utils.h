#pragma once

#include <aegis.hpp>
#include <bot/persistence/repo/settingsRepo.h>

std::string getUsername(const aegis::snowflake& userId, const aegis::snowflake& guildId);
bool isTeacher(const aegis::snowflake& guildId, const aegis::snowflake& userId);