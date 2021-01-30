#pragma once

#include <aegis.hpp>
#include <set>
#include <bot/commands/command.h>

class MuteCommand : public Command {
public:
    MuteCommand() : Command::Command() {
        m_AegisCore->set_on_voice_state_update(std::bind(&MuteCommand::onVoiceStateUpdate, this, std::placeholders::_1));
    }
    void call(const std::vector<std::string>& parameters, CommandContext* ctx);
    CommandInfo getCommandInfo();

private:
    void onVoiceStateUpdate(aegis::gateway::events::voice_state_update obj);

    void changeChannelMuteState(const aegis::snowflake& guildId, const aegis::snowflake& channelId, bool mute);
    void changeMemberMuteState(const aegis::snowflake& userId, const aegis::snowflake& guildId, const aegis::snowflake& channelId, bool mute);
};