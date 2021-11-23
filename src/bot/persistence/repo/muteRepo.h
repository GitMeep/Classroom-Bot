#pragma once

#include <bot/persistence/db.h>
#include <Poco/LRUCache.h>

class MuteRepository {
public:
    MuteRepository();

    std::set<dpp::snowflake> getMutedUsers(const dpp::snowflake& guildId);
    void markUser(const dpp::snowflake& guildId, const dpp::snowflake& user, bool muted);
    bool isUserMarked(const dpp::snowflake& guildId, const dpp::snowflake& userId);

    void markOverride(const dpp::snowflake& guildId, const dpp::snowflake& user, bool overwritten);
    bool isUserOverridden(const dpp::snowflake& guildId, const dpp::snowflake& user);

    void markChannel(const dpp::snowflake& channelId, bool muted);
    bool isChannelMarked(const dpp::snowflake& channelId);

private:
    Poco::LRUCache<dpp::snowflake, bool> m_ChannelCache; // channel id, mute status
    Poco::LRUCache<dpp::snowflake, std::set<dpp::snowflake>> m_UsersCache; // guild, users
    Poco::LRUCache<dpp::snowflake, std::set<dpp::snowflake>> m_UserOverridesCache; // guild, user overrides

    std::shared_ptr<DB> m_DB;
    std::shared_ptr<spdlog::logger> m_Log;
};