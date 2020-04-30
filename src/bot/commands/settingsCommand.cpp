#include "settingsCommad.h"
#include "../utils/utils.h"

const std::string actionMsg = "Please enter a valid option! Options are: ```set, get```";

void SettingsCommand::call(std::vector<std::string> parameters, CurrentCommand current) {
    Command::call(parameters, current);

    std::lock_guard<std::mutex> guard(_mtx);

    if(parameters.size() == 0) {
        _aegisCore->create_message(_current.channelId, actionMsg);
        return;
    }

    std::string verb = parameters[0];
    if(verb == "set") {
        if(parameters.size() < 3) {
            _aegisCore->create_message(_current.channelId, "Command has format: settings set [setting] [value]");
            return;
        }
        set(current.guildId, parameters[1], parameters[2]);
    }

    if(verb == "get") {
        get(current.guildId);
    }
}

void SettingsCommand::set(aegis::snowflake guildId, std::string name, std::string value) {
    if(!isTeacher(_current.guildId, _current.userId, _aegisCore, _bot->_settingsRepo)) {
        _aegisCore->create_message(_current.channelId, "You must have the admin role to use this command.");
        return;
    }
    if(!_bot->_persistence->isEnabled()) {
        _aegisCore->create_message(_current.channelId, "This option is disabled because persistence is disabled.");
        return;
    }

    GuildSettings settings = _bot->_settingsRepo->getSettings(guildId);

    if(name == "prefix") {
        if(value.size() > 1) {
            _aegisCore->create_message(_current.channelId, "Prefix can only be 1 character.");
        }
        settings.prefix = value[0];
    } else if(name == "role") {
        settings.roleName = value;
    } else {
        _aegisCore->create_message(_current.channelId, "Please enter a valid setting. Options are: ```prefix, role```");
        return;
    }

    _bot->_settingsRepo->setSettings(_current.guildId, settings);
    _aegisCore->find_channel(_current.channelId)->create_reaction(aegis::create_reaction_t().message_id(_current.messageId).emoji_text("%E2%9C%85"));
}

void SettingsCommand::get(aegis::snowflake guildId) {
    GuildSettings settings = _bot->_settingsRepo->getSettings(guildId);

    std::stringstream ss;
    ss << "Settings for this server are: \n```";
    ss << "Prefix: " << settings.prefix << "\n";
    ss << "Admin role: " << settings.roleName << "\n```";

    _aegisCore->create_message(_current.channelId, ss.str());
}

bool SettingsCommand::checkPermissions(aegis::permission channelPermissions) {
    return true;
}

CommandInfo SettingsCommand::getCommandInfo() {
    return {
        {"settings", "s"},
        "Change the server's settings",
        {
            "set [setting] [value]: set a setting",
            "get: get settings for your server",
            "Valid settings are: prefix and role"
        },
        "Something went very wrong"
    };
}