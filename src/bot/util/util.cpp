#include <bot/util/util.h>

#include <bot/bot.h>

std::string mentionUser(const dpp::snowflake& userId) {
  return "<@" + std::to_string(userId) + ">";
}