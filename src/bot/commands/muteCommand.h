#pragma once

#include <aegis.hpp>
#include <set>
#include "command.h"

class MuteCommand : public Command {
public:
    MuteCommand() : Command::Command() {
        _aegisCore->set_on_voice_state_update(std::bind(&MuteCommand::onVoiceStateUpdate, this, std::placeholders::_1));
    }
    void call(const std::vector<std::string>& parameters, MessageInfo* current);
    CommandInfo getCommandInfo();

private:
    void onVoiceStateUpdate(aegis::gateway::events::voice_state_update obj);

    void muteChannel(aegis::snowflake channelId);
    void unmuteChannel(aegis::snowflake channelId);
    void changeChannelMuteState(aegis::snowflake channelId, bool mute);
    void changeMemberMuteState(aegis::snowflake userId, aegis::snowflake guildId, aegis::snowflake channelId, bool mute);
};