#pragma once

#include <string>
#include <aegis.hpp>
#include <mutex>

struct GuildSettings {
    std::string prefix; // command prefix, default: ?
    std::string roleName; // teacher role name, default: "Teacher"
};

class Persistence;

class SettingsRepo {
public:
    SettingsRepo(std::shared_ptr<Persistence> persistence);

    GuildSettings getSettings(aegis::snowflake guildId);
    void setSettings(aegis::snowflake guildId, GuildSettings settings);

private:
    std::unordered_map<aegis::snowflake, GuildSettings> _settings;
    std::shared_ptr<Persistence> _persistence;
    std::mutex _settingsMtx;
};