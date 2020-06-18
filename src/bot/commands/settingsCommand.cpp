#include "cbpch.h"

#include "settingsCommad.h"
#include "../bot.h"

void SettingsCommand::call(const std::vector<std::string>& parameters, MessageInfo* current) {
    if(current->isDm) {
        _aegisCore->create_dm_message({current->userId, "Command not supported in DM's."});
        return;
    }

    if(parameters.size() == 0) {
        _aegisCore->create_message(current->channelId, "Command has format: settings set [setting] [value] \nValid settings are `prefix` and `role`.");
        return;
    }

    std::string verb = parameters[0];
    if(verb == "set") {
        if(parameters.size() < 3) {
            _aegisCore->create_message(current->channelId, "Command has format: settings set [setting] [value] \nValid settings are `prefix` and `role`.");
            return;
        }
        set(current, current->guildId, parameters[1], parameters[2]);
    }

    if(verb == "get") {
        get(current, current->guildId);
    }
}

void SettingsCommand::set(MessageInfo* current, aegis::snowflake guildId, std::string name, std::string value) {
    if(!isTeacher(current->guildId, current->userId, _aegisCore, _bot->_settingsRepo)) {
        _aegisCore->create_message(current->channelId, "You must have the admin role to use this command.");
        return;
    }

    Settings settings = _bot->_settingsRepo->get(guildId);

    if(name == "prefix") {
        settings.prefix = value;
    } else if(name == "role") {
        settings.roleName = value;
    } else {
        _aegisCore->create_message(current->channelId, "Please enter a valid setting. Options are: ```prefix, role```");
        return;
    }

    settings.guildId = guildId.get();
    _bot->_settingsRepo->save(settings);
    _aegisCore->find_channel(current->channelId)->create_reaction(aegis::create_reaction_t().message_id(current->messageId).emoji_text("%E2%9C%85"));
}

void SettingsCommand::get(MessageInfo* current, aegis::snowflake guildId) {
    Settings settings = _bot->_settingsRepo->get(guildId);

    nlohmann::json embed {
        {"title", "Settings for " + _aegisCore->find_guild(current->guildId)->get_name()},
        {"fields", nlohmann::json::array({
            {
                {"name", "Prefix"},
                {"value", settings.prefix}
            },
            {
                {"name", "Admin role name"},
                {"value", settings.roleName}
            }
        })}
    };

    _aegisCore->create_message_embed(current->channelId, "", embed);
}

CommandInfo SettingsCommand::getCommandInfo() {
    return {
        "settings",
        {"s"},
        "Configure the server's settings",
        {
            "set [setting] [value]: set a setting (Admin only)",
            "get: see the settings for your server",
            "Settings are: `prefix` and `role`"
        }
    };
}