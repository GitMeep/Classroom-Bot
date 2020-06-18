#pragma once

#include "bot/persistence/model/settings.h"
#include "bot/persistence/cache.h"

class DB;

template <typename T>
class GuildCache;

class SettingsRepository {
public:
    SettingsRepository(const std::shared_ptr<DB>& db);

    Settings get(const aegis::snowflake& guildId);
    void save(const Settings& settings);

private:
    std::shared_ptr<DB> m_DB;
    std::shared_ptr<spdlog::logger> m_Log;
    GuildCache<Settings> m_Cache;

    bool m_InvalidTable = false;
};