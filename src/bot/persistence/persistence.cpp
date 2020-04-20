#include "persistence.h"

#include <aegis.hpp>

using namespace pqxx;

Persistence::Persistence(std::shared_ptr<Config> config)
: _log(spdlog::get("console"))
, _config(config) {

    json conf = _config->getValue("persistence");

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
    _work->commit();
}

// create tables if they don't exist
void Persistence::tryCreateTables() {
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
    _work->exec0(
        "INSERT INTO muted_users "
        "VALUES (" + id.gets() + ");"
    );
}

std::set<aegis::snowflake> Persistence::getMutedUsers() {
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
    _work->exec0(
        "DELETE FROM muted_users "
        "WHERE user_id =" + id.gets() + ";"
    );
}

void Persistence::setGuildSettings(aegis::snowflake guildId, GuildSettings settings) {
    _work->exec0(
        "INSERT INTO guild_settings (guild_id, prefix, role) VALUES(" + _work->quote(guildId.get()) + ", " + _work->quote(settings.prefix) + ", " + _work->quote(settings.roleName) + ") "
        "ON CONFLICT (guild_id) DO UPDATE SET prefix=" + _work->quote(settings.prefix) + ", role=" + _work->quote(settings.roleName) + ";"
    );
}

std::unordered_map<aegis::snowflake, GuildSettings> Persistence::getAllSettings() {
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
