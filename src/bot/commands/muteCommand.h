#pragma once

#include <set>
#include <bot/commands/command.h>

class MuteCommand : public Command {
public:
    MuteCommand() : Command::Command() {
        // m_Cluster->set_on_voice_state_update(std::bind(&MuteCommand::onVoiceStateUpdate, this, std::placeholders::_1));
    }
    void call(int verb, const std::vector<std::string>& parameters, CommandContext* ctx);
    CommandInfo getCommandInfo();

private:
    void onVoiceStateUpdate();

    void onUnmutedUserInMarkedChannel(const dpp::snowflake& userId, const dpp::snowflake& channelId, const dpp::snowflake& guildId);
    void onMarkedUserInUnmarkedChannel(const dpp::snowflake& userId, const dpp::snowflake& channelId,  const dpp::snowflake& guildId);
    void onMarkedUserUnmuted(const dpp::snowflake& userId, const dpp::snowflake& channelId,  const dpp::snowflake& guildId);
    void onMarkedUserMuted(const dpp::snowflake& userId, const dpp::snowflake& channelId,  const dpp::snowflake& guildId);

    void muteAllIn(const dpp::snowflake& channelId, const dpp::snowflake& guildId, bool mute);

    void muteAndMark(const dpp::snowflake& userId, const dpp::snowflake& channelId, const dpp::snowflake& guildId, bool mute);
};