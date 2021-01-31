#pragma once

#include <bot/persistence/db.h>
#include <Poco/LRUCache.h>

class HandRepository {
public:
    HandRepository();

    std::list<aegis::snowflake> get(const aegis::snowflake& channelId);
    void raise(const aegis::snowflake& channelId, const aegis::snowflake& user);
    void lower(const aegis::snowflake& channelId, const aegis::snowflake& user);
    void clear(const aegis::snowflake& channelId);

    void expire();

private:
    Poco::LRUCache<aegis::snowflake, std::list<aegis::snowflake>> m_Cache;

    std::shared_ptr<DB> m_DB;
    std::shared_ptr<spdlog::logger> m_Log;
};