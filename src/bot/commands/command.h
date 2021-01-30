#pragma once

struct CommandInfo {
    std::string name;
    std::vector<std::string> aliases;
    std::string description;
    std::vector<std::string> options;
};

class CommandContext {
public:
    CommandContext(const aegis::snowflake& messageId, const aegis::snowflake& channelId, const aegis::snowflake& guildId, const aegis::snowflake& userId, bool isDM);

    void respond(const std::string& strName);
    void respondEmbed(const std::string& strName, const nlohmann::json& embed);
    void respondUnlocalized(const std::string& message);
    void respondEmbedUnlocalized(const std::string& message, const nlohmann::json& embed);
    void confirm();
    void deny();
    void wait();
    void mute();
    void unmute();
    void waitTyping();

    bool isDM();
    bool isAdmin();

    aegis::snowflake getGuildId();
    aegis::snowflake getMessageId();
    aegis::snowflake getUserId();
    aegis::snowflake getChannelId();

private:
    aegis::snowflake m_MessageId;
    aegis::snowflake m_ChannelId;
    aegis::snowflake m_GuildId;
    aegis::snowflake m_UserId;
    bool m_IsDM;
};

class ClassroomBot;

class Command {
public:
    Command();

    virtual void call(const std::vector<std::string>& parameters, CommandContext* ctx) = 0;

    virtual CommandInfo getCommandInfo() = 0;

protected:
    std::shared_ptr<spdlog::logger> m_Log;
    std::shared_ptr<aegis::core> m_AegisCore;
    ClassroomBot* m_Bot;

};