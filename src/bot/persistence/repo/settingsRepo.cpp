#include <cbpch.h>

#include <bot/bot.h>

#include <bot/persistence/db.h>
#include <bot/persistence/repo/settingsRepo.h>

#include <cstdint>
#include <iostream>
#include <vector>
#include <bsoncxx/json.hpp>
#include <mongocxx/client.hpp>
#include <mongocxx/stdx.hpp>
#include <mongocxx/uri.hpp>
#include <bsoncxx/builder/stream/helpers.hpp>
#include <bsoncxx/builder/stream/document.hpp>
#include <bsoncxx/builder/stream/array.hpp>

using bsoncxx::builder::stream::close_array;
using bsoncxx::builder::stream::close_document;
using bsoncxx::builder::stream::document;
using bsoncxx::builder::stream::finalize;
using bsoncxx::builder::stream::open_array;
using bsoncxx::builder::stream::open_document;

const Settings defaultSettings {"?", "Teacher", "eng"};

SettingsRepository::SettingsRepository() {
    m_Log = ClassroomBot::get().getLog();

    m_DB = ClassroomBot::get().getDatabase();
    this->m_Encryption = m_DB->encryption;
}

Settings SettingsRepository::get(const aegis::snowflake& guildId) {
    if(m_Cache.has(guildId)) {
        return *(m_Cache.get(guildId));
    }

    auto client = m_DB->requestClient();
    auto result = (*client)[m_DB->dbName()]["Settings"].find_one(document{}
        << "guildId" << guildId.gets()
        << finalize
    );

    if(result) {
        std::string prefix, role, lang;

        if(result->view()["prefix"]) {
            prefix = m_Encryption->decrypt(result->view()["prefix"].get_utf8().value.to_string());
        } else {
            prefix = defaultSettings.prefix;
        }
        if(result->view()["roleName"]) {
            role = m_Encryption->decrypt(result->view()["roleName"].get_utf8().value.to_string());
        } else {
            role = defaultSettings.roleName;
        }
        if(result->view()["lang"]) {
            lang = m_Encryption->decrypt(result->view()["lang"].get_utf8().value.to_string());
        } else {
            lang = defaultSettings.lang;
        }
        Settings settings = {
            prefix,
            role,
            lang
        };
        m_Cache.add(guildId, settings);
        return settings;
    }

    m_Cache.add(guildId, defaultSettings);
    return defaultSettings;
}

void SettingsRepository::save(const aegis::snowflake& guildId, const Settings& settings) {
    m_Cache.add(guildId, settings);

    auto client = m_DB->requestClient();
    (*client)[m_DB->dbName()]["Settings"].update_one(
    document{}
        << "guildId" << guildId.gets()
    << finalize,
    document{}
        << "$set" << open_document
            << "prefix" << m_Encryption->encrypt(settings.prefix)
            << "roleName" << m_Encryption->encrypt(settings.roleName)
            << "lang" << m_Encryption->encrypt(settings.lang)
        << close_document
    << finalize,
    mongocxx::options::update().upsert(true)
    );
}
