#include <cbpch.h>

#include <bot/config/config.h>
#include <bot/commands/settingsCommand.h>
#include <bot/bot.h>

void SettingsCommand::call(const std::vector<std::string>& parameters, MessageInfo* current) {
    if(current->isDm) {
        m_AegisCore->create_dm_message({current->userId, "Command not supported in DM's."});
        return;
    }

    if(parameters.size() == 0) {
        m_AegisCore->create_message(current->channelId, "Command has format: settings set [setting] [value] \nValid settings are `prefix` and `role`.");
        return;
    }

    std::string verb = parameters[0];
    if(verb == "set") {
        if(parameters.size() < 3) {
            m_AegisCore->create_message(current->channelId, "Command has format: settings set [setting] [value] \nValid settings are `prefix` and `role`.");
            return;
        }
        set(current, current->guildId, parameters[1], parameters[2]);
    }

    if(verb == "get") {
        get(current, current->guildId);
    }

}

void SettingsCommand::set(MessageInfo* current, const aegis::snowflake& guildId, const std::string& name, const std::string& value) {
    if(!isTeacher(current->guildId, current->userId)) {
        m_AegisCore->create_message(current->channelId, "You must have the admin role to use this command.");
        return;
    }

    if(value.length() >= 3) {
        std::string beginning = value.substr(0, 3);
        if(beginning == "<@!" || beginning == "<@&") {
            m_AegisCore->create_message(current->channelId, "Please enter something without @ in front.");
            return;
        }
    }

    Settings settings = m_Bot->getSettingsRepo()->get(guildId);

    if(name == "prefix") {
        settings.prefix = value;
    } else if(name == "role") {
        settings.roleName = value;
    } else {
        m_AegisCore->create_message(current->channelId, "Please enter a valid setting. Options are: ```prefix, role```");
        return;
    }

    m_Bot->getSettingsRepo()->save(guildId, settings);
    m_AegisCore->find_channel(current->channelId)->create_reaction(aegis::create_reaction_t().message_id(current->messageId).emoji_text("%E2%9C%85"));
}

void SettingsCommand::get(MessageInfo* current, const aegis::snowflake& guildId) {
    Settings settings = m_Bot->getSettingsRepo()->get(guildId);

    nlohmann::json embed {
        {"title", "Settings for " + m_AegisCore->find_guild(current->guildId)->get_name()},
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

    m_AegisCore->create_message_embed(current->channelId, "", embed);
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