#pragma once

#include "bot/persistence/db.h"
#include "bot/persistence/cache.h"

class MuteRepository {
public:
    MuteRepository(const std::shared_ptr<DB>& db);

    std::set<aegis::snowflake> getMutedUsers(const aegis::snowflake& guildId);
    void muteUser(const aegis::snowflake& guildId, const aegis::snowflake& user);
    void unmuteUser(const aegis::snowflake& guildId, const aegis::snowflake& user);

    bool isChannelMuted(const aegis::snowflake& channelId);
    void muteChannel(const aegis::snowflake& channelId);
    void unmuteChannel(const aegis::snowflake& channelId);


private:
    std::shared_ptr<DB> m_DB;
    std::shared_ptr<spdlog::logger> m_Log;
    GuildCache<std::set<aegis::snowflake>> m_UserCache;
    std::set<aegis::snowflake> m_MutedChannels;

    bool m_InvalidUserTable = false;
    bool m_InvalidChannelTable = false;
};