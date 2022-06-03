#pragma once

#include <dpp/snowflake.h>

#include <bot/persistence/db.h>
#include <Poco/LRUCache.h>

class MuteRepo {
public:
    MuteRepo(const MuteRepo&) = delete;

    static std::set<dpp::snowflake> getMutedUsers(const dpp::snowflake& guildId);
    static void markUser(const dpp::snowflake& guildId, const dpp::snowflake& user, bool muted);
    static bool isUserMarked(const dpp::snowflake& guildId, const dpp::snowflake& userId);

    static void markOverride(const dpp::snowflake& guildId, const dpp::snowflake& user, bool overwritten);
    static bool isUserOverridden(const dpp::snowflake& guildId, const dpp::snowflake& user);

    static void markChannel(const dpp::snowflake& channelId, bool muted);
    static bool isChannelMarked(const dpp::snowflake& channelId);

private:
    static Poco::LRUCache<dpp::snowflake, bool> m_ChannelCache; // channel id, mute status
    static Poco::LRUCache<dpp::snowflake, std::set<dpp::snowflake>> m_UsersCache; // guild, users
    static Poco::LRUCache<dpp::snowflake, std::set<dpp::snowflake>> m_UserOverridesCache; // guild, user overrides
};