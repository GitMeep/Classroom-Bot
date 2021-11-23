#pragma once

#include <bot/persistence/repo/settingsRepo.h>

std::string getUsername(const dpp::snowflake& userId, const dpp::snowflake& guildId);
bool isAdmin(const dpp::snowflake& guildId, const dpp::snowflake& userId);