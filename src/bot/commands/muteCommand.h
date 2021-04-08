#pragma once

#include <aegis.hpp>
#include <set>
#include <bot/commands/command.h>

class MuteCommand : public Command {
public:
    MuteCommand() : Command::Command() {
        m_AegisCore->set_on_voice_state_update(std::bind(&MuteCommand::onVoiceStateUpdate, this, std::placeholders::_1));
    }
    void call(int verb, const std::vector<std::string>& parameters, CommandContext* ctx);
    CommandInfo getCommandInfo();

private:
    void onVoiceStateUpdate(aegis::gateway::events::voice_state_update obj);

    void onUnmutedUserInMarkedChannel(const aegis::snowflake& userId, const aegis::snowflake& channelId, const aegis::snowflake& guildId);
    void onMarkedUserInUnmarkedChannel(const aegis::snowflake& userId, const aegis::snowflake& channelId,  const aegis::snowflake& guildId);
    void onMarkedUserUnmuted(const aegis::snowflake& userId, const aegis::snowflake& channelId,  const aegis::snowflake& guildId);
    void onMarkedUserMuted(const aegis::snowflake& userId, const aegis::snowflake& channelId,  const aegis::snowflake& guildId);

    void muteAllIn(const aegis::snowflake& channelId, const aegis::snowflake& guildId, bool mute);

    void muteAndMark(const aegis::snowflake& userId, const aegis::snowflake& channelId, const aegis::snowflake& guildId, bool mute);
};