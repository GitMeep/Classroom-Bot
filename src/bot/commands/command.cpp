#include <cbpch.h>

#include <bot/commands/command.h>
#include <bot/localization/localization.h>
#include <bot/bot.h>

Command::Command()
: m_Log(ClassroomBot::get().getLog())
, m_AegisCore(ClassroomBot::get().getAegis())
, m_Bot(&ClassroomBot::get()) {}

CommandContext::CommandContext(const aegis::snowflake& messageId, const aegis::snowflake& channelId, const aegis::snowflake& guildId, const aegis::snowflake& userId, bool isDM) 
 : m_MessageId(messageId)
 , m_ChannelId(channelId)
 , m_GuildId(guildId)
 , m_UserId(userId)
 , m_IsDM(isDM) {}

void CommandContext::respond(const std::string& strName) {
    // get server language setting here, for now it's just english
    std::string response = ClassroomBot::get().getLocalization()->getString("eng", strName);
    respondUnlocalized(response);
}

void CommandContext::respondEmbed(const std::string& strName, const nlohmann::json& embed) {
    // get server language setting here, for now it's just english
    std::string message = ClassroomBot::get().getLocalization()->getString("eng", strName);
    respondEmbedUnlocalized(message, embed);
}

void CommandContext::respondUnlocalized(const std::string& message) {
    ClassroomBot::get().getAegis()->find_channel(m_ChannelId)->create_message(message);
}

void CommandContext::respondEmbedUnlocalized(const std::string& message, const nlohmann::json& embed) {
    ClassroomBot::get().getAegis()->find_channel(m_ChannelId)->create_message_embed(message, embed);
}

void CommandContext::confirm() {
    ClassroomBot::get().getAegis()->find_channel(m_ChannelId)->create_reaction(m_MessageId, "✅");
}

void CommandContext::deny() {
    ClassroomBot::get().getAegis()->find_channel(m_ChannelId)->create_reaction(m_MessageId, "🚫");
}

void CommandContext::wait() {
    ClassroomBot::get().getAegis()->find_channel(m_ChannelId)->create_reaction(m_MessageId, "⌛");
}

void CommandContext::mute() {
    ClassroomBot::get().getAegis()->find_channel(m_ChannelId)->create_reaction(m_MessageId, "🔇");
}

void CommandContext::unmute() {
    ClassroomBot::get().getAegis()->find_channel(m_ChannelId)->create_reaction(m_MessageId, "🔈");
}

void CommandContext::waitTyping() {
    this->wait();
    ClassroomBot::get().getAegis()->find_channel(m_ChannelId)->trigger_typing_indicator();
}

bool CommandContext::isDM() {
    return this->m_IsDM;
}

bool CommandContext::isAdmin() {
    auto guild = ClassroomBot::get().getAegis()->find_guild(m_GuildId);
    auto settings = ClassroomBot::get().getSettingsRepo();
    std::string adminRole = settings->get(m_GuildId).roleName;
    auto role = guild->find_role(adminRole);

    if(m_UserId == guild->get_owner()) return true;

    if(!role) return false;

    return guild->member_has_role(m_UserId, role->role_id);
}

aegis::snowflake CommandContext::getGuildId() {
    return m_GuildId;
}

aegis::snowflake CommandContext::getMessageId() {
    return m_MessageId;
}

aegis::snowflake CommandContext::getUserId() {
    return m_UserId;
}

aegis::snowflake CommandContext::getChannelId() {
    return m_ChannelId;
}
