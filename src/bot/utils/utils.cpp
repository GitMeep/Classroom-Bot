#include <bot/utils/utils.h>

#define ADMIN_ROLE "Teacher"

std::string getUsername(aegis::user &user, aegis::snowflake guildId) {
    std::string username = user.get_name(guildId);
    if(username.size()==0) username = user.get_username();
    return username;
}

bool isTeacher(aegis::gateway::events::message_create message) {
    aegis::snowflake guildId = message.channel.get_guild_id();
    std::vector<aegis::snowflake> roles = message.get_user().get_guild_info(guildId).roles;
    std::vector<aegis::snowflake>::iterator it = roles.begin();
    bool hasRole = false;
    while(it != roles.end()) {
        if(message.channel.get_guild().get_role(*it).name == ADMIN_ROLE) {
            hasRole = true;
            break;
        }
        it++;
    }
    return hasRole;
}