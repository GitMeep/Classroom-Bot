#include "./utils.h"
#include <mutex>

std::mutex utilsMutex;

std::string getUsername(aegis::snowflake userId, aegis::snowflake guildId, std::shared_ptr<aegis::core> core) {
    std::lock_guard<std::mutex> guard(utilsMutex);
    aegis::user* user = core->find_guild(guildId)->find_member(userId);
    std::string username = user->get_name(guildId);
    if(username.size()==0) username = user->get_username();
    return username;
}

bool isTeacher(aegis::snowflake guildId, aegis::snowflake userId, std::shared_ptr<aegis::core> core, std::shared_ptr<SettingsRepository> settings) {
    std::lock_guard<std::mutex> guard(utilsMutex);
    std::string adminRole = settings->get(guildId).roleName;

    auto roles = core->find_guild(guildId)->find_member(userId)->get_guild_info(guildId).roles;
    auto it = roles.begin();
    bool hasRole = false;
    while(it != roles.end()) {
        if(core->find_guild(guildId)->get_role(*it).name == adminRole) {
            hasRole = true;
            break;
        }
        it++;
    }
    return hasRole;
}