#pragma once

#include <pqxx/pqxx>
#include <spdlog/spdlog.h>
#include <aegis.hpp>

#include "../config/config.h"
#include "settings.h"

class Persistence {
public:
    explicit Persistence(std::shared_ptr<Config> config);
    ~Persistence();

    // settings
    void setGuildSettings(aegis::snowflake guildId, GuildSettings settings);
    std::unordered_map<aegis::snowflake, GuildSettings> getAllSettings();
    bool isEnabled();

    void setUserMute(aegis::snowflake id);
    std::set<aegis::snowflake> getMutedUsers();
    void unsetUserMute(aegis::snowflake id);

protected:
    void tryCreateTables();
    bool _enabled;
    std::unique_ptr<pqxx::connection> _conn;
    std::unique_ptr<pqxx::nontransaction> _work;
    std::shared_ptr<spdlog::logger> _log;
    std::shared_ptr<Config> _config;
};