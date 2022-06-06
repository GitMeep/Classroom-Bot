#include "command.h"

#include <dpp/fmt/format.h>
#include <bot/localization/localization.h>

// CommandContext

CommandContext::CommandContext(const dpp::interaction_create_t& event, InteractionType type) :
type(type),
event(event),
m_LangCode(event.command.locale) {}

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
  return Localization::getString(name, m_LangCode);
}

// Command
const Command::CommandSpec& Command::spec() {
  return m_Spec;
}
