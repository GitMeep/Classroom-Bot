#pragma once

#include <dpp/dpp.h>

#include <string>

const std::string mentionUser(const dpp::snowflake& userId);
const std::string nickOrUsername(const dpp::snowflake& userId, const dpp::snowflake& guildId);