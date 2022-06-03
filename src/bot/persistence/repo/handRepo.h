#pragma once

#include <dpp/snowflake.h>

#include <bot/persistence/db.h>
#include <Poco/LRUCache.h>

class HandRepo {
public:
    HandRepo(const HandRepo&) = delete;

    static std::list<dpp::snowflake> get(const dpp::snowflake& channelId);
    static void raise(const dpp::snowflake& channelId, const dpp::snowflake& user);
    static void lower(const dpp::snowflake& channelId, const dpp::snowflake& user);
    static void clear(const dpp::snowflake& channelId);

    static void expire();

private:
    static Poco::LRUCache<dpp::snowflake, std::list<dpp::snowflake>> m_Cache;
};