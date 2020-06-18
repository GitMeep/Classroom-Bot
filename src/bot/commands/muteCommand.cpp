#include "muteCommand.h"
#include "../bot.h"
#include "bot/persistence/repo/muteRepo.h"

#include "../utils/utils.h"

void MuteCommand::call(const std::vector<std::string>& parameters, MessageInfo* current) {
    if(current->isDm) {
        _aegisCore->create_dm_message(current->userId, "Command not supported in DM's");
        return;
    }

    if(!isTeacher(current->guildId, current->userId, _aegisCore, _bot->_settingsRepo)) {
        _aegisCore->find_channel(current->channelId)->create_message("You are not a teacher.");
        return;
    }
    
    auto voiceStates = _aegisCore->find_guild(current->guildId)->get_voicestates();
    if(!voiceStates.count(current->userId)) {
        _aegisCore->find_channel(current->channelId)->create_message("You are not in a voice channel.");
        return;
    }

    aegis::snowflake voiceChannel = voiceStates[current->userId].channel_id;

    if(_bot->m_MuteRepo->isChannelMuted(voiceChannel)) {
        changeChannelMuteState(voiceChannel, false);
        _aegisCore->find_channel(current->channelId)->create_reaction(current->messageId, "%F0%9F%94%88"); // 🔈
    } else {
        changeChannelMuteState(voiceChannel, true);
        _aegisCore->find_channel(current->channelId)->create_reaction(current->messageId, "%F0%9F%94%87"); // 🔇
    }
}

void MuteCommand::onVoiceStateUpdate(aegis::gateway::events::voice_state_update obj) {
    bool channelMute = _bot->m_MuteRepo->isChannelMuted(obj.channel_id);
    if(channelMute && !obj.mute) { // if someone joins a muted channel, and they are not mute already, mute them
        changeMemberMuteState(obj.user_id, obj.guild_id, obj.channel_id, true);
        return;
    }

    if(!channelMute) { // user left or joined an unmuted channel
        auto mutedUsers = _bot->m_MuteRepo->getMutedUsers(obj.guild_id);
        // unmute user if they are muted and it was the bot that muted them
        if(obj.mute && mutedUsers.count(obj.user_id) && (obj.channel_id.get() != 0)) // if someone we muted joins an unmuted channel, unmute them (if someone leaves a channel, the channel id will be 0, so don't do anything in that case)
            changeMemberMuteState(obj.user_id, obj.guild_id, obj.channel_id, false);

    }
}

void MuteCommand::changeChannelMuteState(aegis::snowflake channelId, bool mute) {
    if(mute) {
        if(_bot->m_MuteRepo->isChannelMuted(channelId))
            return;
        _bot->m_MuteRepo->muteChannel(channelId);
    } else {
        _bot->m_MuteRepo->unmuteChannel(channelId);
    }
    aegis::snowflake guildId = _aegisCore->find_channel(channelId)->get_guild_id();

    // update everyone currently in the voice channel
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
            _bot->m_MuteRepo->muteUser(guildId, userId);
        } else {
            _bot->m_MuteRepo->unmuteUser(guildId, userId);
        }
        _aegisCore->find_guild(guildId)->modify_guild_member(userId, aegis::lib::nullopt, mute, aegis::lib::nullopt, aegis::lib::nullopt, aegis::lib::nullopt);
    }
}

CommandInfo MuteCommand::getCommandInfo() {
    return {
        "mute",
        {"m"},
        "(Admin only) Toggles mute on the voice channel that you are in. Everyone in a muted channel, except teachers, get server muted. This way you can easily mute a class who wont stop talking.",
        {}
    };
}