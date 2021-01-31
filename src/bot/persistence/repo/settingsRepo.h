#pragma once

#include <bot/persistence/model/settings.h>
#include <bot/persistence/db.h>
#include <Poco/LRUCache.h>

class SettingsRepository {
public:
    SettingsRepository();

    Settings get(const aegis::snowflake& guildId);
    void save(const aegis::snowflake& guildId, const Settings& settings);

private:
    Poco::LRUCache<aegis::snowflake, Settings> m_Cache;

    std::shared_ptr<DB> m_DB;
    std::shared_ptr<Encryption> m_Encryption;
    std::shared_ptr<spdlog::logger> m_Log;
};