#include <bot/bot.h>
#include <bot/persistence/db.h>
#include <bot/persistence/hash/sha256.h>
#include <bot/persistence/repo/settingsRepo.h>

#include <bsoncxx/builder/stream/array.hpp>
#include <bsoncxx/builder/stream/document.hpp>
#include <bsoncxx/builder/stream/helpers.hpp>
#include <bsoncxx/json.hpp>
#include <cstdint>
#include <iostream>
#include <mongocxx/client.hpp>
#include <mongocxx/stdx.hpp>
#include <mongocxx/uri.hpp>
#include <vector>

using bsoncxx::builder::stream::close_array;
using bsoncxx::builder::stream::close_document;
using bsoncxx::builder::stream::document;
using bsoncxx::builder::stream::finalize;
using bsoncxx::builder::stream::open_array;
using bsoncxx::builder::stream::open_document;

const Settings SettingsRepo::defaultSettings {"en-US"};

// static members
Poco::LRUCache<dpp::snowflake, Settings> SettingsRepo::m_Cache;

Settings SettingsRepo::get(const dpp::snowflake& guildId) {
  if (m_Cache.has(guildId)) {
    return *(m_Cache.get(guildId));
  }

  auto client = DB::requestClient();
  auto result = (*client)[DB::name()]["Settings"].find_one(document {} << "guildId" << base64Sha256(guildId) << finalize);

  if (result) {
    std::string lang;

    if (result->view()["lang"]) {
      lang = std::string(result->view()["lang"].get_utf8().value);
    } else {
      lang = defaultSettings.lang;
    }
    Settings settings = {
        lang};
    m_Cache.add(guildId, settings);
    return settings;
  }

  m_Cache.add(guildId, defaultSettings);
  return defaultSettings;
}

void SettingsRepo::save(const dpp::snowflake& guildId, const Settings& settings) {
  m_Cache.add(guildId, settings);

  auto client = DB::requestClient();
  (*client)[DB::name()]["Settings"].update_one(
      document {}
          << "guildId" << base64Sha256(guildId)
          << finalize,
      document {}
          << "$set" << open_document
          << "lang" << DB::encrypt(settings.lang)
          << close_document
          << finalize,
      mongocxx::options::update().upsert(true)
  );
}
