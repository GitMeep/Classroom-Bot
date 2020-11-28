#pragma once

#include <bot/persistence/model/settings.h>
#include <bot/persistence/db.h>

class DB;

template <typename T>
class GuildCache;

class SettingsRepository {
public:
    SettingsRepository();

    Settings get(const aegis::snowflake& guildId);
    void save(const aegis::snowflake& guildId, const Settings& settings);

private:
    std::shared_ptr<DB> m_DB;
    std::shared_ptr<Encryption> m_Encryption;
    std::shared_ptr<spdlog::logger> m_Log;
};