#include <bot/commands/command.h>
#include <bot/localization/localization.h>
#include <bot/persistence/repo/settingsRepo.h>
#include <bot/bot.h>

Command::Command()
: m_Log(ClassroomBot::getLog())
, m_Cluster(ClassroomBot::getCluster())
, m_Bot(&ClassroomBot::getBot()) {}

CommandContext::CommandContext(const dpp::snowflake& messageId, const dpp::snowflake& channelId, const dpp::snowflake& guildId, const dpp::snowflake& userId, bool isDM, const Settings& settings)
 : m_MessageId(messageId)
 , m_ChannelId(channelId)
 , m_GuildId(guildId)
 , m_UserId(userId)
 , m_IsDM(isDM)
 , m_Settings(settings) {}

void CommandContext::respond(const std::string& strName) {
    // get server language setting here, for now it's just english
    std::string response = ClassroomBot::getLocalization()->getString(m_Settings.lang, strName);
    respondUnlocalized(response);
}

void CommandContext::respondEmbed(const std::string& strName, nlohmann::json& embed) {
    // get server language setting here, for now it's just english
    std::string message = ClassroomBot::getLocalization()->getString(m_Settings.lang, strName);
    respondEmbedUnlocalized(message, embed);
}

void CommandContext::respondUnlocalized(const std::string& message) {
    ClassroomBot::getCluster()->message_create(dpp::message(m_ChannelId, message));
}

void CommandContext::respondEmbedUnlocalized(const std::string& message, nlohmann::json& embed) {
    ClassroomBot::getCluster()->message_create(dpp::message(m_ChannelId, dpp::embed(&embed)));
}

void CommandContext::react(const std::string& emote) {
    ClassroomBot::getCluster()->message_add_reaction(m_MessageId, m_ChannelId, emote);
}

void CommandContext::confirm() {
    react("✅");
}

void CommandContext::deny() {
    react("🚫");
}

void CommandContext::wait() {
    react("⌛");
}

void CommandContext::mute() {
    react("🔇");
}

void CommandContext::unmute() {
    react("🔈");
}

void CommandContext::waitTyping() {
    this->wait();
    ClassroomBot::getCluster()->channel_typing(m_ChannelId);
}

bool CommandContext::isDM() {
    return this->m_IsDM;
}

bool CommandContext::isAdmin() {
    auto guild = dpp::find_guild(m_GuildId);
    auto settings = ClassroomBot::getBot().getSettingsRepo();
    std::string adminRoleName = settings->get(m_GuildId).roleName;
    
    // Find role
    dpp::role* adminRole = nullptr;
    dpp::role* tmpPointer = nullptr;
    for(const auto& role : guild->roles) {
        tmpPointer = dpp::find_role(role);
        if(tmpPointer->name == adminRoleName) {
            adminRole = tmpPointer;
            break;
        }
    }

    if(m_UserId == guild->owner_id) return true;

    if(adminRole == nullptr) return false;

    for(const auto& role : guild->members[m_UserId].roles) {
        if(role == adminRole->id) return true;
    }

    return false; 
}

dpp::snowflake CommandContext::getGuildId() {
    return m_GuildId;
}

dpp::snowflake CommandContext::getMessageId() {
    return m_MessageId;
}

dpp::snowflake CommandContext::getUserId() {
    return m_UserId;
}

dpp::snowflake CommandContext::getChannelId() {
    return m_ChannelId;
}

Settings CommandContext::getSettings() {
    return m_Settings;
}
