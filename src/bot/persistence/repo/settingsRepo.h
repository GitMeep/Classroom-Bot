#pragma once

#include <dpp/dpp.h>

#include <bot/persistence/model/settings.h>
#include <bot/persistence/db.h>
#include <Poco/LRUCache.h>

class SettingsRepo {
public:
    SettingsRepo(const SettingsRepo&) = delete;

    static Settings get(const dpp::snowflake& guildId);
    static void save(const dpp::snowflake& guildId, const Settings& settings);

private:
    static Poco::LRUCache<dpp::snowflake, Settings> m_Cache;
    static const Settings defaultSettings;
};