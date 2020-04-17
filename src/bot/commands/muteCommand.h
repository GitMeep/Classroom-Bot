#pragma once

#include <aegis.hpp>
#include <set>
#include "command.h"

class MuteCommand : public Command {
public:
    explicit MuteCommand(ClassroomBot* classroomBot) : Command::Command(classroomBot) {
        _aegisCore->set_on_voice_state_update(std::bind(&MuteCommand::onVoiceStateUpdate, this, std::placeholders::_1));
    }
    void call(std::vector<std::string> parameters, CurrentCommand current);
    CommandInfo getCommandInfo();
    bool checkPermissions(aegis::permission channelPermissions);

private:
    std::set<aegis::snowflake> _mutedChannels; // channels that are mute enforced
    std::set<aegis::snowflake> _mutedUsers; // users that the bot is responsible for muting
    std::unordered_map<aegis::snowflake, aegis::snowflake> _teachers; // maps teachers user id to the channel they are in
    std::mutex _lock;

    void onVoiceStateUpdate(aegis::gateway::events::voice_state_update obj);
    void muteChannel(aegis::snowflake channelId);
    void unmuteChannel(aegis::snowflake channelId);
    void changeChannelMuteState(aegis::snowflake channelId, bool mute);
    void changeMemberMuteState(aegis::snowflake userId, aegis::snowflake guildId, aegis::snowflake channelId, bool mute);
    bool doesChannelHaveTeacher(aegis::snowflake channelId);
};