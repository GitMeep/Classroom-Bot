#include "settingsCommand.h"

#include <bot/bot.h>
#include <bot/config/config.h>
#include <bot/localization/localization.h>
#include <bot/persistence/repo/settingsRepo.h>
#include <fmt/format.h>

const std::string adminSelectMenuId = "settings_admin_select";

SettingsCommand::SettingsCommand() {
  m_Spec.commands.reserve(1); // Set to number of commands to prevent the vector from moving the data and invalidating the references
  dpp::slashcommand& command = m_Spec.commands.emplace_back(Localization::getString("settings_cmd_settings"), Localization::getString("settings_cmd_settings_desc"), NULL)
                                   .set_default_permissions(dpp::p_manage_messages);

  dpp::command_option optionLanguage(dpp::co_sub_command, Localization::getString("settings_cmd_settings_option_language"), Localization::getString("settings_cmd_settings_option_language_desc"));

  for (const auto& lang : Localization::getLanguages()) {
    const std::string& currentLangCode = lang.first;

    if (currentLangCode == "en-US") continue;

    addLocalizationIfExists(command, currentLangCode, "settings_cmd_settings", "settings_cmd_settings_desc");
    addLocalizationIfExists(optionLanguage, currentLangCode, "settings_cmd_settings_option_language", "settings_cmd_settings_option_language_desc");
  }

  command.add_option(optionLanguage);

  m_Spec.selectMenuIds.emplace_back(adminSelectMenuId);
}

void SettingsCommand::command(const CommandContext& ctx) {
  dpp::command_interaction ci = ctx.event.command.get_command_interaction();

  const std::string& subcommandName = ci.options[0].name;

  if (subcommandName == Localization::getString("settings_cmd_settings_option_language")) language(ctx);
  else {
    LOG_WARN("Settings: unknown command: " + subcommandName);
    ctx.replyUnlocalized("Unknown command", true);
  }
}

void SettingsCommand::selectClick(const CommandContext& ctx) {
  dpp::select_click_t& event = ctx.selectClick();

  std::string lang = event.values[0];

  SettingsRepo::save(ctx.guildId(), {lang});
  ctx.replyUnlocalized(
      fmt::format(
          Localization::getString("settings_changed_language", lang),
          fmt::arg("language", Localization::getLanguageName(lang))
      ),
      true
  );
}

void SettingsCommand::language(const CommandContext& ctx) {
  dpp::component languageSelectMenu = dpp::component()
                                          .set_type(dpp::cot_selectmenu)
                                          .set_id(adminSelectMenuId)
                                          .set_placeholder(
                                              ctx.localize("settings_language")
                                          );

  auto serverLang = SettingsRepo::get(ctx.guildId()).lang;

  for (auto& lang : Localization::getLanguages()) {
    dpp::select_option option = dpp::select_option()
                                    .set_label(lang.second)
                                    .set_value(lang.first)
                                    .set_default(lang.first == serverLang);

    std::string translator = Localization::getTranslator(lang.first);
    if (translator != "") {
      option.set_description(
          ctx.localize("settings_translator") + " " + translator
      );
    }

    if (Localization::isPartial(lang.first)) {
      option.set_label(
          option.label + " ⚠️ Partially translated ⚠️"
      );
    }

    languageSelectMenu.add_select_option(option);
  }

  ctx.replyUnlocalized(
      dpp::message()
          .set_content(ctx.localize("settings_cmd_settings_desc"))
          .add_component(
              dpp::component().add_component(
                  languageSelectMenu
              )
          )
          .set_flags(dpp::m_ephemeral)
  );
}
