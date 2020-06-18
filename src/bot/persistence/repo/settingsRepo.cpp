#include "cbpch.h"

#include "bot/persistence/db.h"
#include "settingsRepo.h"

const Settings defaultSettings {0, "?", "Teacher"};

SettingsRepository::SettingsRepository(const std::shared_ptr<DB>& db) : m_DB(db) {
    m_Log = spdlog::get("classroombot");

    bool tableValid = m_DB->verifyTable("Settings", {
        {"guildId", "bigint"},
        {"prefix", "text"},
        {"roleName", "text"}
    });

    if (!tableValid) {
        m_Log->warn("Settings table invalid, won't save settings to database");
        m_InvalidTable = true;
        return;
    }

    m_DB->prepare("getSettings", "SELECT * FROM \"Settings\" WHERE \"guildId\" = $1");
    m_DB->prepare("updateSettings", "UPDATE \"Settings\" SET \"prefix\" = $1, \"roleName\" = $2 WHERE \"guildId\" = $3;");
    m_DB->prepare("insertSettings", "INSERT INTO \"Settings\" VALUES ($1, $2, $3)");
}

Settings SettingsRepository::get(const aegis::snowflake& guildId) {
    if(m_Cache.has(guildId)) {
        return m_Cache.get(guildId);
    } else if(m_InvalidTable) {
        return defaultSettings;
    }

    pqxx::result res = m_DB->execPrep("getSettings", guildId.gets());

    if (res.size() == 0) { // not found
        return defaultSettings;
    }

    pqxx::row row = res[0];

    Settings settings;
    settings.guildId = row[0].as<unsigned long>();
    settings.prefix = row[1].c_str();
    settings.roleName = row[2].c_str();

    m_Cache.save(guildId, settings);

    return settings;
}

void SettingsRepository::save(const Settings& settings) {
    if(!m_InvalidTable) {
        auto res = m_DB->execPrep("updateSettings", settings.prefix, settings.roleName, settings.guildId);
    
        if (res.affected_rows() == 0) { // settings dont exist yet, create them in the DB
            m_DB->execPrep("insertSettings", settings.guildId, settings.prefix, settings.roleName);
        }
    }

    m_Cache.save(settings.guildId, settings);
}