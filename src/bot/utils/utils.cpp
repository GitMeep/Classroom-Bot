#include <bot/utils/utils.h>
#include <mutex>
#include <bot/bot.h>
#include <fmt/format.h>

std::string getUsername(const aegis::snowflake& userId, const aegis::snowflake& guildId) {
    auto core = ClassroomBot::get().getAegis();

    aegis::user* user = core->find_guild(guildId)->find_member(userId);
    if (user == nullptr) return "Unknown user";
    std::string username = user->get_name(guildId);
    if(username.size()==0) username = user->get_username();
    return username;
}

bool isTeacher(const aegis::snowflake& guildId, const aegis::snowflake& userId) {
    auto guild = ClassroomBot::get().getAegis()->find_guild(guildId);
    auto settings = ClassroomBot::get().getSettingsRepo();
    std::string adminRole = settings->get(guildId).roleName;
    auto role = guild->find_role(adminRole);

    if(userId == guild->get_owner()) return true;

    if(!role) return false;

    return guild->member_has_role(userId, role->role_id);
}