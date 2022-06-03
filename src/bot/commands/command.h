#pragma once

#include <string>
#include <vector>
#include <spdlog/spdlog.h>
#include <dpp/dpp.h>
#include <bot/persistence/model/settings.h>

struct CommandInfo {
    std::string name;
    std::string localName; // localized string name
    std::vector<std::string> aliases;
    std::string description;
    std::vector<std::string> optionDescriptions;
    std::vector<std::string> options;
    bool noDM;
};

class CommandContext {
public:
    CommandContext(const dpp::snowflake& messageId, const dpp::snowflake& channelId, const dpp::snowflake& guildId, const dpp::snowflake& userId, bool isDM, const Settings& settings);

    void respond(const std::string& strName);
    void respondEmbed(const std::string& strName, nlohmann::json& embed);
    void respondUnlocalized(const std::string& message);
    void respondEmbedUnlocalized(const std::string& message, nlohmann::json& embed);
    void react(const std::string& emote);
    void confirm();
    void deny();
    void wait();
    void mute();
    void unmute();
    void waitTyping();

    bool isDM();
    bool isAdmin();

    dpp::snowflake getGuildId();
    dpp::snowflake getMessageId();
    dpp::snowflake getUserId();
    dpp::snowflake getChannelId();
    Settings getSettings();

private:
    dpp::snowflake m_MessageId;
    dpp::snowflake m_ChannelId;
    dpp::snowflake m_GuildId;
    dpp::snowflake m_UserId;
    Settings m_Settings;
    bool m_IsDM;
};

class ClassroomBot;

class Command {
public:
    virtual void call(int verb, const std::vector<std::string>& parameters, CommandContext* ctx) = 0;

    virtual CommandInfo getCommandInfo() = 0;
};