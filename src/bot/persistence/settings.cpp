#include "settings.h"
#include "persistence.h"

GuildSettings defaultSettings {
    "?",
    "Teacher"
};

SettingsRepo::SettingsRepo(std::shared_ptr<Persistence> persistence) : _persistence(persistence) {
    _settings = persistence->getAllSettings();
}

GuildSettings SettingsRepo::getSettings(aegis::snowflake guildId) {
    std::lock_guard<std::mutex> guard(_settingsMtx);
    if(!_settings.count(guildId)) return defaultSettings;
    return _settings[guildId];
}

void SettingsRepo::setSettings(aegis::snowflake guildId, GuildSettings settings) {
    std::lock_guard<std::mutex> guard(_settingsMtx);
    _persistence->setGuildSettings(guildId, settings);
    _settings[guildId] = settings;
}