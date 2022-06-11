#include "command.h"

#include <bot/bot.h>
#include <dpp/fmt/format.h>
#include <bot/localization/localization.h>

// CommandContext

CommandContext::CommandContext(const dpp::interaction_create_t& event, InteractionType type) :
type(type),
event(event),
m_UserLangCode(event.command.locale),
m_GuildLangCode(event.command.guild_locale) {}

const dpp::snowflake& CommandContext::userId() const {
  return event.command.usr.id;
}

const dpp::snowflake& CommandContext::channelId() const {
  return event.command.channel_id;
}

const dpp::snowflake& CommandContext::guildId() const {
  return event.command.guild_id;
}

const dpp::snowflake CommandContext::contextUser() const {
  return type == ContextUser ? ((const dpp::user_context_menu_t&)event).get_user().id : 0;
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

// Command
const Command::CommandSpec& Command::spec() {
  return m_Spec;
}

// global
void addLocalizationIfExists(dpp::slashcommand& command, const std::string& language, const std::string& name, const std::string& description) {
  if(Localization::hasString(name, language) && Localization::hasString(description, language)) {
    command.add_localization(language, Localization::getString(name, language), Localization::getString(description, language));
  }
}

void addLocalizationIfExists(dpp::command_option& option, const std::string& language, const std::string& name, const std::string& description) {
  if(Localization::hasString(name, language) && Localization::hasString(description, language)) {
    option.add_localization(language, Localization::getString(name, language), Localization::getString(description, language));
  }
}

// provide defauly implementations that do nothing so derived classes don't have to overwrite every one
void Command::command(const CommandContext& ctx) {}
void Command::userContext(const CommandContext& ctx) {}
void Command::messageContext(const CommandContext& ctx) {}
void Command::buttonClick(const CommandContext& ctx) {}
void Command::selectClick(const CommandContext& ctx) {}
void Command::formSubmit(const CommandContext& ctx) {}
