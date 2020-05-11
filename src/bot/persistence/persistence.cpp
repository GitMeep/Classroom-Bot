#include "persistence.h"

#include <aegis.hpp>

using namespace pqxx;

Persistence::Persistence(std::shared_ptr<Config> config)
: _log(spdlog::get("aegis"))
, _config(config) {

    json conf = _config->getValue("persistence");

    std::string enableValue;
    if(conf.count("enable") && !conf["enable"].is_null()) {
        enableValue = conf["enable"];
    } else {
        throw std::runtime_error("Config option persistence.url not found!");
    }
    if(enableValue == "true") {
        _log->info("Persistence enabled");
        _enabled = true;
    } else {
        _log->info("Persistence disabled");
        _enabled = false;
        return;
    }

    std::string url;
    if(conf.count("url") && !conf["url"].is_null()) {
        url = conf["url"];
    } else {
        throw std::runtime_error("Config option persistence.url not found!");
    }

    try {
        _log->info("Attempting to connect to database!");
        _conn = std::make_unique<connection>(connection(url));
        _log->info("Succesfully connected to database!");
        
        _work = std::make_unique<nontransaction>(*_conn);
        tryCreateTables();
    } catch (std::exception const & e) {
        _log->error("Could not connect to database: " + std::string(e.what()));
        throw(new std::runtime_error("Database connection failed!"));
    }
}

Persistence::~Persistence() {
    if(_enabled) {
        _work->commit();
    }
}

bool Persistence::isEnabled() {
    return _enabled;
}

// create tables if they don't exist
void Persistence::tryCreateTables() {
    if(!_enabled) return;
    _work->exec0(
        "CREATE TABLE IF NOT EXISTS muted_users "
        "("
            "user_id bigint NOT NULL,"
            "CONSTRAINT muted_users_pkey PRIMARY KEY (user_id)"
        ");\n"
        "CREATE TABLE IF NOT EXISTS guild_settings "
        "("
            "guild_id bigint NOT NULL,"
            "prefix \"char\" NOT NULL,"
            "role text COLLATE pg_catalog.\"default\","
            "CONSTRAINT prefixes_pkey PRIMARY KEY (guild_id)"
        ");"
    );
}

void Persistence::setUserMute(aegis::snowflake id) {
    if(!_enabled) return;
    _work->exec0(
        "INSERT INTO muted_users "
        "VALUES (" + id.gets() + ")"
        "ON CONFLICT DO NOTHING;"
    );
}

std::set<aegis::snowflake> Persistence::getMutedUsers() {
    if(!_enabled) return std::set<aegis::snowflake>();
    pqxx::result res = _work->exec(
        "SELECT * FROM muted_users;"
    );

    std::set<aegis::snowflake> users;

    for(auto row : res) {
        aegis::snowflake snowflake(row[0].as<int64_t>());
        users.emplace(snowflake);
    }

    return users;
}

void Persistence::unsetUserMute(aegis::snowflake id) {
    if(!_enabled) return;
    _work->exec0(
        "DELETE FROM muted_users "
        "WHERE user_id =" + id.gets() + ";"
    );
}

void Persistence::setGuildSettings(aegis::snowflake guildId, GuildSettings settings) {
    if(!_enabled) return;
    _work->exec0(
        "INSERT INTO guild_settings (guild_id, prefix, role) VALUES(" + _work->quote(guildId.get()) + ", " + _work->quote(settings.prefix) + ", " + _work->quote(settings.roleName) + ") "
        "ON CONFLICT (guild_id) DO UPDATE SET prefix=" + _work->quote(settings.prefix) + ", role=" + _work->quote(settings.roleName) + ";"
    );
}

std::unordered_map<aegis::snowflake, GuildSettings> Persistence::getAllSettings() {
    if(!_enabled) return std::unordered_map<aegis::snowflake, GuildSettings>();
    pqxx::result res = _work->exec(
        "SELECT * FROM guild_settings;"
    );

    std::unordered_map<aegis::snowflake, GuildSettings> settingsMap;

    for(auto row : res) {
        aegis::snowflake id(row[0].as<int64_t>());

        std::optional<std::string> prefix = row[1].get<std::string>();
        std::optional<std::string> roleName = row[2].get<std::string>();

        if(!prefix.has_value()) {
            prefix = "?";
        }

        if(!roleName.has_value()) {
            roleName = "Teacher";
        }

        GuildSettings settings {
            prefix.value().substr(0,1),
            roleName.value()
        };
        settingsMap.emplace(id, settings);
    }
    
    return settingsMap;
}
