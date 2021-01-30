#include <cbpch.h>

#include <bot/config/config.h>
#include <bot/commands/settingsCommand.h>
#include <bot/bot.h>

void SettingsCommand::call(const std::vector<std::string>& parameters, CommandContext* ctx) {
    if(ctx->isDM()) {
        ctx->respond("no_dm");
        return;
    }

    if(parameters.size() == 0) {
        ctx->respond("settings_usage");
        return;
    }

    std::string verb = parameters[0];
    if(verb == "set") {
        if(parameters.size() < 3) {
            ctx->respond("settings_usage");
            return;
        }
        set(ctx, ctx->getGuildId(), parameters[1], parameters[2]);
    }

    if(verb == "get") {
        get(ctx, ctx->getGuildId());
    }

}

void SettingsCommand::set(CommandContext* ctx, const aegis::snowflake& guildId, const std::string& name, const std::string& value) {
    if(!ctx->isAdmin()) {
        ctx->respond("admin_required");
        return;
    }

    if(value.length() >= 3) {
        std::string beginning = value.substr(0, 3);
        if(beginning == "<@!" || beginning == "<@&") {
            ctx->respond("no_att");
            return;
        }
    }

    Settings settings = m_Bot->getSettingsRepo()->get(guildId);

    if(name == "prefix") {
        settings.prefix = value;
    } else if(name == "role") {
        settings.roleName = value;
    } else {
        ctx->respond("settings_usage");
        return;
    }

    m_Bot->getSettingsRepo()->save(guildId, settings);
    ctx->confirm();
}

void SettingsCommand::get(CommandContext* ctx, const aegis::snowflake& guildId) {
    Settings settings = m_Bot->getSettingsRepo()->get(guildId);

    nlohmann::json embed {
        {"title", "Settings for " + m_AegisCore->find_guild(ctx->getGuildId())->get_name()},
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

    m_AegisCore->create_message_embed(ctx->getChannelId(), "", embed);
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