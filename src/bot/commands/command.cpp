#include "command.h"

#include <bot/bot.h>
#include <bot/localization/localization.h>
#include <fmt/format.h>

// CommandContext

const dpp::snowflake& CommandContext::userId() const {
  return event.command.usr.id;
}

const dpp::snowflake& CommandContext::channelId() const {
  return event.command.channel_id;
}

const dpp::snowflake& CommandContext::guildId() const {
  return event.command.guild_id;
}

void CommandContext::confirm() const {
  event.reply(dpp::message("✅").set_flags(dpp::m_ephemeral));
}

void CommandContext::deny() const {
  event.reply(dpp::message("🚫").set_flags(dpp::m_ephemeral));
}

void CommandContext::wait() const {
  event.reply(dpp::message("⌛").set_flags(dpp::m_ephemeral));
}

void CommandContext::mute() const {
  event.reply(dpp::message("🔇").set_flags(dpp::m_ephemeral));
}

void CommandContext::unmute() const {
  event.reply(dpp::message("🔈").set_flags(dpp::m_ephemeral));
}

void CommandContext::reply() const {
  event.reply();
}

void CommandContext::dialog(const dpp::interaction_modal_response& mr) const {
  event.dialog(mr);
}

void CommandContext::replyUnlocalized(const std::string& message, bool ephemeral) const {
  event.reply(dpp::message(message).set_flags(ephemeral ? dpp::m_ephemeral : 0));
}

void CommandContext::replyUnlocalized(const dpp::message& message) const {
  event.reply(message);
}

void CommandContext::replyLocalized(const std::string& name, bool ephemeral) const {
  event.reply(dpp::message(localize(name)).set_flags(ephemeral ? dpp::m_ephemeral : 0));
}

const std::string& CommandContext::localize(const std::string& name) const {
  return Localization::getString(name, m_UserLangCode);
}

const std::string& CommandContext::localizeGuild(const std::string& name) const {
  // TODO: Guild locale is only for community servers, it should be overwritten with a per-server bot setting so normal servers can set their preferred locale for the bot as well
  return Localization::getString(name, m_GuildLangCode);
}

void CommandContext::replyUnlocalizedChannel(dpp::message message) const {
  ClassroomBot::cluster().message_create(message.set_channel_id(channelId()));
}

const std::string& CommandContext::userLanguage() const {
  return m_UserLangCode;
}

dpp::autocomplete_t& CommandContext::autocomplete() const {
  if (eventType != AutoComplete) throw std::runtime_error("Doesn't contain the correct event type");
  return (dpp::autocomplete_t&)event;
}

dpp::button_click_t& CommandContext::buttonClick() const {
  if (eventType != ButtonClick) throw std::runtime_error("Doesn't contain the correct event type");
  return (dpp::button_click_t&)event;
}

dpp::message_context_menu_t& CommandContext::messageContext() const {
  if (eventType != MessageContext) throw std::runtime_error("Doesn't contain the correct event type");
  return (dpp::message_context_menu_t&)event;
}

dpp::user_context_menu_t& CommandContext::userContext() const {
  if (eventType != UserContext) throw std::runtime_error("Doesn't contain the correct event type");
  return (dpp::user_context_menu_t&)event;
}

dpp::form_submit_t& CommandContext::formSubmit() const {
  if (eventType != FormSubmit) throw std::runtime_error("Doesn't contain the correct event type");
  return (dpp::form_submit_t&)event;
}

dpp::select_click_t& CommandContext::selectClick() const {
  if (eventType != SelectClick) throw std::runtime_error("Doesn't contain the correct event type");
  return (dpp::select_click_t&)event;
}

dpp::slashcommand_t CommandContext::slashCommand() const {
  if (eventType != SlashCommand) throw std::runtime_error("Doesn't contain the correct event type");
  return (dpp::slashcommand_t&)event;
}

// Command
const Command::CommandSpec& Command::spec() {
  return m_Spec;
}

static std::string pleaseImplement = "Please implement this function yourself";
// provide default implementations that do nothing so derived classes don't have to overwrite every one
void Command::command(const CommandContext& ctx) {
  LOG_WARN(pleaseImplement);
}
void Command::userContext(const CommandContext& ctx) {
  LOG_WARN(pleaseImplement);
}
void Command::messageContext(const CommandContext& ctx) {
  LOG_WARN(pleaseImplement);
}
void Command::buttonClick(const CommandContext& ctx) {
  LOG_WARN(pleaseImplement);
}
void Command::selectClick(const CommandContext& ctx) {
  LOG_WARN(pleaseImplement);
}
void Command::formSubmit(const CommandContext& ctx) {
  LOG_WARN(pleaseImplement);
}

// global
void addLocalizationIfExists(dpp::slashcommand& command, const std::string& language, const std::string& name, const std::string& description) {
  if (Localization::hasString(name, language) && Localization::hasString(description, language)) {
    command.add_localization(language, Localization::getString(name, language), Localization::getString(description, language));
  }
}

void addLocalizationIfExists(dpp::command_option& option, const std::string& language, const std::string& name, const std::string& description) {
  if (Localization::hasString(name, language) && Localization::hasString(description, language)) {
    option.add_localization(language, Localization::getString(name, language), Localization::getString(description, language));
  }
}
