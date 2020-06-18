#pragma once

#include "bot/persistence/db.h"
#include "bot/persistence/cache.h"

class HandRepository {
public:
    HandRepository(const std::shared_ptr<DB>& db);

    std::list<aegis::snowflake> get(const aegis::snowflake& guildId);
    void raise(const aegis::snowflake& guildId, const aegis::snowflake& user);
    void lower(const aegis::snowflake& guildId, const aegis::snowflake& user);
    void clear(const aegis::snowflake& guildId);


private:
    std::shared_ptr<DB> m_DB;
    std::shared_ptr<spdlog::logger> m_Log;
    GuildCache<std::list<aegis::snowflake>> m_Cache;

    bool m_InvalidTable = false;
};