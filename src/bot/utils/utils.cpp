#include <bot/utils/utils.h>
#include <mutex>
#include <bot/bot.h>
#include <fmt/format.h>

std::string getUsernameREST(const dpp::snowflake& userId, const dpp::snowflake& guildId) {
    Poco::Net::HTTPSClientSession session("discord.com", 443);
    Poco::Net::HTTPRequest req("GET", "/api/guilds/" + guildId.gets() + "/members/" + userId.gets());
    req.setCredentials("Bot", ClassroomBot::getCluster()->get_token());

    session.sendRequest(req);
    Poco::Net::HTTPResponse res;
    std::string body(std::istreambuf_iterator<char>(session.receiveResponse(res)), {});

    auto member = nlohmann::json::parse(body);
    if(member["nick"].is_null()) return member["user"]["username"];
    
    return member["nick"];
}

std::string getUsername(const dpp::snowflake& userId, const dpp::snowflake& guildId) {
    auto core = ClassroomBot::getCluster();

    aegis::user* user = core->find_guild(guildId)->find_member(userId);
    if (user == nullptr) {
        spdlog::get("classroombot")->error("Couldn't get username of member " + userId.gets() + " in guild " + guildId.gets());
        return getUsernameREST(userId, guildId);
    }
    std::string username = user->get_name(guildId);
    if(username.size()==0) username = user->get_username();
    return username;
}

bool isAdmin(const dpp::snowflake& guildId, const dpp::snowflake& userId) {
    auto guild = ClassroomBot::getCluster()->find_guild(guildId);
    auto settings = ClassroomBot::getBot().getSettingsRepo();
    std::string adminRole = settings->get(guildId).roleName;
    auto role = guild->find_role(adminRole);

    if(userId == guild->get_owner()) return true;

    if(!role) return false;

    return guild->member_has_role(userId, role->role_id);
}