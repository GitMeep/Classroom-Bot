#pragma once

#include <bot/persistence/db.h>
#include <Poco/LRUCache.h>

class MuteRepository {
public:
    MuteRepository();

    std::set<aegis::snowflake> getMutedUsers(const aegis::snowflake& guildId);
    void markUser(const aegis::snowflake& guildId, const aegis::snowflake& user, bool muted);
    bool isUserMarked(const aegis::snowflake& guildId, const aegis::snowflake& userId);

    void markOverride(const aegis::snowflake& guildId, const aegis::snowflake& user, bool overwritten);
    bool isUserOverridden(const aegis::snowflake& guildId, const aegis::snowflake& user);

    void markChannel(const aegis::snowflake& channelId, bool muted);
    bool isChannelMarked(const aegis::snowflake& channelId);

private:
    Poco::LRUCache<aegis::snowflake, bool> m_ChannelCache; // channel id, mute status
    Poco::LRUCache<aegis::snowflake, std::set<aegis::snowflake>> m_UsersCache; // guild, users
    Poco::LRUCache<aegis::snowflake, std::set<aegis::snowflake>> m_UserOverridesCache; // guild, user overrides

    std::shared_ptr<DB> m_DB;
    std::shared_ptr<spdlog::logger> m_Log;
};