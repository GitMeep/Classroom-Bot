#pragma once

#include <aegis.hpp>
#include <set>
#include "command.h"

class MuteCommand : public Command {
public:
    MuteCommand(std::shared_ptr<aegis::core> aegisCore) : Command::Command(aegisCore->log, aegisCore) {
        _aegisCore->set_on_voice_state_update(std::bind(&MuteCommand::onVoiceStateUpdate, this, std::placeholders::_1));
    }
    void call(std::vector<std::string> parameters, CurrentCommand current);
    CommandInfo getCommandInfo();

private:
    std::set<aegis::snowflake> _mutedChannels; // channels that are mute enforced
    std::unordered_map<aegis::snowflake, std::unordered_map<aegis::snowflake, aegis::snowflake>> _mutedUsers; // guild_id, {user_id, channel_id}
    std::set<aegis::snowflake> _unmuteQueue;
    std::mutex _lock;

    void onVoiceStateUpdate(aegis::gateway::events::voice_state_update obj);
    void muteChannel(aegis::snowflake channelId);
    void unmuteChannel(aegis::snowflake channelId);
    void changeChannelMuteState(aegis::snowflake channelId, bool muted);
    void changeMemberMuteState(aegis::snowflake userId, aegis::snowflake guildId, aegis::snowflake channelId, bool muted);
    bool isChannelEmpty(aegis::snowflake channelId);
    bool doesChannelHaveTeacher(aegis::snowflake channelId);
};