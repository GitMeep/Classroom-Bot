#include "muteCommand.h"

#include "../utils/utils.h"

void MuteCommand::call(std::vector<std::string> parameters, CurrentCommand current) {
    std::lock_guard<std::mutex> guard(_lock);
    Command::call(parameters, current);

    if(!isTeacher(current.guildId, current.userId, _aegisCore, _bot->_settingsRepo)) {
        _aegisCore->find_channel(current.channelId)->create_message("You are not a teacher!");
        return;
    }
    
    auto voiceStates = _aegisCore->find_guild(current.guildId)->get_voicestates();
    if(!voiceStates.count(current.userId)) {
        _aegisCore->find_channel(current.channelId)->create_message("You are not in a voice channel!");
        return;
    }

    aegis::snowflake voiceChannel = voiceStates[current.userId].channel_id;

    if(_mutedChannels.count(voiceChannel)) {
        changeChannelMuteState(voiceChannel, false);
        _aegisCore->find_channel(current.channelId)->create_reaction(current.messageId, "%F0%9F%94%88"); // 🔈
    } else {
        changeChannelMuteState(voiceChannel, true);
        _aegisCore->find_channel(current.channelId)->create_reaction(current.messageId, "%F0%9F%94%87"); // 🔇
    }
}

void MuteCommand::loadFromDB() {
    auto users = _bot->_persistence->getMutedUsers();
    _mutedUsers = users;
}

void MuteCommand::onVoiceStateUpdate(aegis::gateway::events::voice_state_update obj) {
    if(_mutedChannels.count(obj.channel_id) && !obj.mute) { // if someone joins a muted channel, and they are not mute already, mute them
        changeMemberMuteState(obj.user_id, obj.guild_id, obj.channel_id, true);
        return;
    }

    if(!_mutedChannels.count(obj.channel_id)) { // user left or joined an unmuted channel
        // unmute user
        if(obj.mute && _mutedUsers.count(obj.user_id) && (obj.channel_id.get() != 0)) // if someone we muted joins an unmuted channel, unmute them
            changeMemberMuteState(obj.user_id, obj.guild_id, obj.channel_id, false);

        // if a teacher leaves, check if the channel they were in has any more teachers, if not unmute it.
        if(isTeacher(obj.guild_id, obj.user_id, _aegisCore, _bot->_settingsRepo) && _teachers.count(obj.user_id)) {
            aegis::snowflake channel = _teachers[obj.user_id];
            if(!doesChannelHaveTeacher(channel)) {
                changeChannelMuteState(channel, false);
            }
            _teachers.erase(obj.user_id);
        }
    }
}

bool MuteCommand::doesChannelHaveTeacher(aegis::snowflake channelId) {
    aegis::snowflake guildId = _aegisCore->find_channel(channelId)->get_guild_id();

    auto voiceStates = _aegisCore->find_guild(guildId)->get_voicestates();
    auto it = voiceStates.begin();
    while(it != voiceStates.end()) {
        if(it->second.channel_id == channelId && isTeacher(guildId, it->second.user_id, _aegisCore, _bot->_settingsRepo))
            return true;
        it++;
    }
    return false;
}

void MuteCommand::changeChannelMuteState(aegis::snowflake channelId, bool mute) {
    if(mute) {
        if(_mutedChannels.count(channelId))
            return;
        _mutedChannels.emplace(channelId);
    } else {
        _mutedChannels.erase(channelId);
    }
    aegis::snowflake guildId = _aegisCore->find_channel(channelId)->get_guild_id();

    auto voiceStates = _aegisCore->find_guild(guildId)->get_voicestates();
    auto it = voiceStates.begin();
    while(it != voiceStates.end()) {
        if(it->second.channel_id == channelId) {
            if(!(it->second.mute && mute)) // if we are muting people, and the current one is already mute, skip them
                changeMemberMuteState(it->second.user_id, guildId, channelId, mute);
        }
        it++;
    }
}

void MuteCommand::changeMemberMuteState(aegis::snowflake userId, aegis::snowflake guildId, aegis::snowflake channelId, bool mute) {
    if(!isTeacher(guildId, userId, _aegisCore, _bot->_settingsRepo)) {// don't touch teachers
        if(mute) {
            _mutedUsers.emplace(userId);
            _bot->_persistence->setUserMute(userId);
        } else {
            _mutedUsers.erase(userId);
            _bot->_persistence->unsetUserMute(userId);
        }
        _aegisCore->find_guild(guildId)->modify_guild_member(userId, aegis::lib::nullopt, mute, aegis::lib::nullopt, aegis::lib::nullopt, aegis::lib::nullopt);
    } else {
        _teachers.emplace(userId, channelId);
    }
}

bool MuteCommand::checkPermissions(aegis::permission channelPermissions) {
    return
    channelPermissions.can_voice_mute() &&
    channelPermissions.can_add_reactions();
}

CommandInfo MuteCommand::getCommandInfo() {
    return {
        {"mute", "m"},
        "(Admin only) Toggles mute on the voice channel that the caller is in. Everyone in a muted channel, except teachers, get server muted. When all teachers leave a channel, it is automatically unmuted.",
        {},
        "I need permission to mute members and add reactions to use this command"
    };
}