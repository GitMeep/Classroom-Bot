#pragma once

#include <bot/persistence/db.h>
#include <Poco/LRUCache.h>

class HandRepository {
public:
    HandRepository();

    std::list<dpp::snowflake> get(const dpp::snowflake& channelId);
    void raise(const dpp::snowflake& channelId, const dpp::snowflake& user);
    void lower(const dpp::snowflake& channelId, const dpp::snowflake& user);
    void clear(const dpp::snowflake& channelId);

    void expire();

private:
    Poco::LRUCache<dpp::snowflake, std::list<dpp::snowflake>> m_Cache;

    std::shared_ptr<DB> m_DB;
    std::shared_ptr<spdlog::logger> m_Log;
};