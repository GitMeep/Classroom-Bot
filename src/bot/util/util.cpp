#include <bot/util/util.h>

#include <bot/bot.h>

const std::string mentionUser(const dpp::snowflake& userId) {
  return "<@" + std::to_string(userId) + ">";
}

const std::string nickOrUsername(const dpp::snowflake& userId, const dpp::snowflake& guildId) {
  auto member = ClassroomBot::cluster().guild_get_member_sync(guildId, userId);

  std::string nickOrName = member.nickname;
  if(nickOrName == "") nickOrName = ClassroomBot::cluster().user_get_sync(member.user_id).username;

  return nickOrName;
}
