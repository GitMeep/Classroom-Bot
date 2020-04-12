#include "muteCommand.h"

#include "../utils/utils.h"

void MuteCommand::call(std::vector<std::string> parameters, CurrentCommand current) {
    _lock.lock();
    Command::call(parameters, current);
    
    auto voiceStates = _aegisCore->find_guild(current.guildId)->get_voicestates();
    if(!voiceStates.count(current.userId)) {
        _aegisCore->find_channel(current.channelId)->create_message("You are not in a voice channel!");
        return;
    }

    aegis::snowflake voiceChannel = voiceStates[current.userId].channel_id;

    if(_mutedChannels.count(voiceChannel)) {
        unmuteChannel(voiceChannel);
        _aegisCore->find_channel(current.channelId)->create_reaction(current.messageId, "%F0%9F%94%88"); // 🔈
    } else {
        muteChannel(voiceChannel);
        _aegisCore->find_channel(current.channelId)->create_reaction(current.messageId, "%F0%9F%94%87"); // 🔇
    }
    
    _lock.unlock();
}

void MuteCommand::muteChannel(aegis::snowflake channelId) {
    if(_mutedChannels.count(channelId))
        return;

    _mutedChannels.emplace(channelId);
    changeChannelMuteState(channelId, true);
}

void MuteCommand::unmuteChannel(aegis::snowflake channelId) {
    _mutedChannels.erase(channelId);
    changeChannelMuteState(channelId, false);
}

void MuteCommand::onVoiceStateUpdate(aegis::gateway::events::voice_state_update obj) {
    if(_unmuteQueue.count(obj.user_id)) {
        _unmuteQueue.erase(obj.user_id);
        if(!_mutedChannels.count(obj.channel_id))
            changeMemberMuteState(obj.user_id, obj.guild_id, obj.channel_id, false);
    }

    if(_mutedChannels.count(obj.channel_id) && !obj.mute) { // if someone joins a muted channel, mute them
        changeMemberMuteState(obj.user_id, obj.guild_id, obj.channel_id, true);
        return;
    }

    if(!_mutedChannels.count(obj.channel_id) && _mutedUsers[obj.guild_id].count(obj.user_id)) { // user left or joined an unmuted channel
        // check if the channel has any teachers left, if not, unmute it
        aegis::snowflake channelId = _mutedUsers[obj.guild_id][obj.user_id];
        if(!doesChannelHaveTeacher(channelId))
            unmuteChannel(channelId);

        if(obj.channel_id.get() == 0) { // user left, queue them up for unmuting
            _unmuteQueue.emplace(obj.user_id);
            return;
        }

        // unmute user
        changeMemberMuteState(obj.user_id, obj.guild_id, _mutedUsers[obj.guild_id][obj.user_id], false);
        return;
    }
}

bool MuteCommand::doesChannelHaveTeacher(aegis::snowflake channelId) {
    aegis::snowflake guildId = _aegisCore->find_channel(channelId)->get_guild_id();

    auto voiceStates = _aegisCore->find_guild(guildId)->get_voicestates();
    auto it = voiceStates.begin();
    while(it != voiceStates.end()) {
        if(it->second.channel_id == channelId && isTeacher(guildId, it->second.user_id, _aegisCore))
            return true;
        it++;
    }
    return false;
}

void MuteCommand::changeChannelMuteState(aegis::snowflake channelId, bool muted) {
    aegis::snowflake guildId = _aegisCore->find_channel(channelId)->get_guild_id();

    auto voiceStates = _aegisCore->find_guild(guildId)->get_voicestates();
    auto it = voiceStates.begin();
    while(it != voiceStates.end()) {
        if(it->second.channel_id == channelId) {
            changeMemberMuteState(it->second.user_id, guildId, channelId, muted);
        }
        it++;
    }
}

void MuteCommand::changeMemberMuteState(aegis::snowflake userId, aegis::snowflake guildId, aegis::snowflake channelId, bool muted) {
    if(muted) {
        _mutedUsers[guildId][userId] = channelId;
    } else {
        _mutedUsers[guildId].erase(userId);
        if(_mutedUsers[guildId].size() < 1)
            _mutedUsers.erase(guildId);
    }
    if(!isTeacher(guildId, userId, _aegisCore)) // don't touch teachers
        _aegisCore->find_guild(guildId)->modify_guild_member(userId, aegis::lib::nullopt, muted, aegis::lib::nullopt, aegis::lib::nullopt, aegis::lib::nullopt);
}

CommandInfo MuteCommand::getCommandInfo() {
    return {
        {"mute", "m"},
        "This command toggles mute on a channel. Everyone in a muted channel, except teachers, get server muted. When all teachers leave a channel, it is automatically unmuted.",
        {}
    };
}