#pragma once

#include <bot/persistence/repo/settingsRepo.h>

std::string getUsername(const aegis::snowflake& userId, const aegis::snowflake& guildId);
bool isAdmin(const aegis::snowflake& guildId, const aegis::snowflake& userId);