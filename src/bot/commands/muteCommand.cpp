#include <bot/commands/muteCommand.h>
#include <bot/bot.h>
#include <bot/persistence/repo/muteRepo.h>

#include <bot/utils/utils.h>

void MuteCommand::call(const std::vector<std::string>& parameters, MessageInfo* current) {
    if(current->isDm) {
        m_AegisCore->create_dm_message(current->userId, "Command not supported in DM's");
        return;
    }

    if(!isTeacher(current->guildId, current->userId)) {
        m_AegisCore->find_channel(current->channelId)->create_message("You are not a teacher.");
        return;
    }
    
    auto voiceStates = m_AegisCore->find_guild(current->guildId)->get_voicestates();
    if(!voiceStates.count(current->userId)) {
        m_AegisCore->find_channel(current->channelId)->create_message("You are not in a voice channel.");
        return;
    }

    aegis::snowflake voiceChannel = voiceStates[current->userId].channel_id;

    if(m_Bot->getMuteRepo()->isChannelMuted(voiceChannel)) {
        changeChannelMuteState(current->guildId, voiceChannel, false);
        m_AegisCore->find_channel(current->channelId)->create_reaction(current->messageId, "%F0%9F%94%88"); // 🔈
    } else {
        changeChannelMuteState(current->guildId, voiceChannel, true);
        m_AegisCore->find_channel(current->channelId)->create_reaction(current->messageId, "%F0%9F%94%87"); // 🔇
    }
}

void MuteCommand::onVoiceStateUpdate(aegis::gateway::events::voice_state_update obj) {
    bool channelMute = m_Bot->getMuteRepo()->isChannelMuted(obj.channel_id);
    if(channelMute && !obj.mute) { // if someone joins a muted channel, and they are not mute already, mute them
        changeMemberMuteState(obj.user_id, obj.guild_id, obj.channel_id, true);
        return;
    }

    if(!channelMute) { // user left or joined an unmuted channel
        auto mutedUsers = m_Bot->getMuteRepo()->getMutedUsers(obj.guild_id);
        // unmute user if they are muted and it was the bot that muted them
        if(obj.mute && mutedUsers.count(obj.user_id) && (obj.channel_id.get() != 0)) // if someone we muted joins an unmuted channel, unmute them (if someone leaves a channel, the channel id will be 0, so don't do anything in that case)
            changeMemberMuteState(obj.user_id, obj.guild_id, obj.channel_id, false);

    }
}

void MuteCommand::changeChannelMuteState(const aegis::snowflake& guildId, const aegis::snowflake& channelId, bool mute) {
    if(mute) {
        if(m_Bot->getMuteRepo()->isChannelMuted(channelId))
            return;
        m_Bot->getMuteRepo()->muteChannel(channelId);
    } else {
        m_Bot->getMuteRepo()->unmuteChannel(channelId);
    }

    // update everyone currently in the voice channel
    auto voiceStates = m_AegisCore->find_guild(guildId)->get_voicestates();
    auto it = voiceStates.begin();
    while(it != voiceStates.end()) {
        if(it->second.channel_id == channelId) {
            if(!(it->second.mute && mute)) // if we are muting people, and the current one is already mute, skip them
                changeMemberMuteState(it->second.user_id, guildId, channelId, mute);
        }
        it++;
    }
}

void MuteCommand::changeMemberMuteState(const aegis::snowflake& userId, const aegis::snowflake& guildId, const aegis::snowflake& channelId, bool mute) {
    if(!isTeacher(guildId, userId)) {// don't touch teachers
        if(mute) {
            m_Bot->getMuteRepo()->muteUser(guildId, userId);
        } else {
            m_Bot->getMuteRepo()->unmuteUser(guildId, userId);
        }
        m_AegisCore->find_guild(guildId)->modify_guild_member(userId, {}, mute, {}, {}, {});
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