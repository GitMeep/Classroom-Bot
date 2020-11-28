#pragma once

#include <bot/persistence/db.h>

class MuteRepository {
public:
    MuteRepository();

    std::set<aegis::snowflake> getMutedUsers(const aegis::snowflake& guildId);
    void muteUser(const aegis::snowflake& guildId, const aegis::snowflake& user);
    void unmuteUser(const aegis::snowflake& guildId, const aegis::snowflake& user);

    bool isChannelMuted(const aegis::snowflake& channelId);
    void muteChannel(const aegis::snowflake& channelId);
    void unmuteChannel(const aegis::snowflake& channelId);
    void cleanCache();

private:
    std::shared_ptr<DB> m_DB;
    std::shared_ptr<spdlog::logger> m_Log;
};