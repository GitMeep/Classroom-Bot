#include <cbpch.h>

#include <bot/config/config.h>
#include <bot/commands/settingsCommand.h>
#include <bot/bot.h>
#include <bot/localization/localization.h>

void SettingsCommand::call(int verb, const std::vector<std::string>& parameters, CommandContext* ctx) {
    switch(verb) {
    case 1: // get
        get(ctx);
        break;
    case 2: // set
        set(ctx, parameters);
        break;
    default:
    case 0:
        ctx->respond("settings_usage");
        break;
    }
}

void SettingsCommand::set(CommandContext* ctx, const std::vector<std::string>& parameters) {
    if(!ctx->isAdmin()) {
        ctx->respond("admin_required");
        return;
    }

    if(parameters.size() < 3) {
        ctx->respond("settings_usage");
        return;
    }
    std::string
        name = parameters[1],
        value = parameters[2];

    LocHelper loc(m_Bot->getLocalization(), ctx->getSettings().lang);
    std::string
        localizedPrefix = loc.get("settings_setting_prefix"),
        localizedRole = loc.get("settings_setting_role"),
        localizedLang = loc.get("settings_setting_lang");

    if(value.length() >= 3) {
        std::string beginning = value.substr(0, 3);
        if(beginning == "<@!" || beginning == "<@&") {
            ctx->respond("no_att");
            return;
        }
    }

    Settings settings = m_Bot->getSettingsRepo()->get(ctx->getGuildId());

    if(name == localizedPrefix) {
        settings.prefix = value;
    } else if(name == localizedRole) {
        settings.roleName = value;
    } else if(name == localizedLang) {
        if(m_Bot->getLocalization()->hasLanguage(value)) {
            settings.lang = value;
        } else {
            std::string langsString = m_Bot->getLocalization()->getString(ctx->getSettings().lang, "unknown_language") + "\nDisclaimer: Translation are made by the community, and may not be accurate.\n";
            std::string translator;
            auto langs = m_Bot->getLocalization()->getLanguages();
            auto lang = langs.begin();
            while(lang != langs.end()) {
                langsString += "`" + lang->first + "`: " + lang->second;
                translator = m_Bot->getLocalization()->getTranslator(lang->first);
                if(translator != "") langsString += " - " + translator;
                langsString += "\n";
                lang++;
            }
            ctx->respondUnlocalized(langsString);
            return;
        }
    } else {
        ctx->respond("settings_usage");
        return;
    }

    m_Bot->getSettingsRepo()->save(ctx->getGuildId(), settings);
    ctx->confirm();
}

void SettingsCommand::get(CommandContext* ctx) {
    Settings settings = m_Bot->getSettingsRepo()->get(ctx->getGuildId());
    if(settings.lang == "") settings.lang = "eng";

    LocHelper loc(m_Bot->getLocalization(), ctx->getSettings().lang);

    nlohmann::json embed {
        {"title", loc.get("settings_for") + " " + m_AegisCore->find_guild(ctx->getGuildId())->get_name()},
        {"fields", nlohmann::json::array({
            {
                {"name", loc.get("settings_prefix")},
                {"value", settings.prefix}
            },
            {
                {"name", loc.get("settings_role_name")},
                {"value", settings.roleName}
            },
            {
                {"name", loc.get("settings_language")},
                {"value", m_Bot->getLocalization()->getLanguageName(settings.lang)}
            }
        })}
    };

    m_AegisCore->create_message_embed(ctx->getChannelId(), "", embed);
}

CommandInfo SettingsCommand::getCommandInfo() {
    return {
        "settings",
        "settings_cmd",
        {"settings_alias"},
        "settings_desc",
        {
            "settings_option_set_desc",
            "settings_option_get_desc"
        },
        {
            "settings_option_get",
            "settings_option_set"
        },
        true
    };
}