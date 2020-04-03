#include "./utils.h"

#define ADMIN_ROLE "Teacher"

std::string getUsername(aegis::snowflake userId, aegis::snowflake guildId, std::shared_ptr<aegis::core> core) {
    aegis::user* user = core->find_guild(guildId)->find_member(userId);
    std::string username = user->get_name(guildId);
    if(username.size()==0) username = user->get_username();
    return username;
}

bool isTeacher(aegis::snowflake guildId, aegis::snowflake userId, std::shared_ptr<aegis::core> core) {
    auto roles = core->find_guild(guildId)->find_member(userId)->get_guild_info(guildId).roles;
    auto it = roles.begin();
    bool hasRole = false;
    while(it != roles.end()) {
        if(core->find_guild(guildId)->get_role(*it).name == ADMIN_ROLE) {
            hasRole = true;
            break;
        }
        it++;
    }
    return hasRole;
}