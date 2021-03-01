#include <bot/commands/muteCommand.h>
#include <bot/bot.h>
#include <bot/persistence/repo/muteRepo.h>

#include <bot/utils/utils.h>

void MuteCommand::call(int verb, const std::vector<std::string>& parameters, CommandContext* ctx) {
    if(ctx->isDM()) {
        ctx->respond("no_dm");
        return;
    }

    if(!ctx->isAdmin()) {
        ctx->respond("admin_required");
        return;
    }
    
    auto voiceStates = m_AegisCore->find_guild(ctx->getGuildId())->get_voicestates();
    if(!voiceStates.count(ctx->getUserId()) && parameters.size() < 1) {
        ctx->respond("not_in_vc");
        return;
    }

    aegis::snowflake voiceChannel;
    if(parameters.size() < 1) {
        voiceChannel = voiceStates[ctx->getUserId()].channel_id;
    } else {
        try {
            long long id = std::stoll(parameters[0]);
            voiceChannel = aegis::snowflake(id);
        } catch (std::invalid_argument& e) {
            ctx->respond("invalid_id");
            return;
        }
        if(
        m_AegisCore->find_guild(ctx->getGuildId())->find_channel(voiceChannel) == nullptr ||
        m_AegisCore->find_guild(ctx->getGuildId())->find_channel(voiceChannel)->get_type() != aegis::gateway::objects::channel::channel_type::Voice) {
            ctx->respond("vc_not_exist");
            return;
        }
    }
    

    if(m_Bot->getMuteRepo()->isChannelMuted(voiceChannel)) {
        changeChannelMuteState(ctx->getGuildId(), voiceChannel, false);
        ctx->unmute();
    } else {
        changeChannelMuteState(ctx->getGuildId(), voiceChannel, true);
        ctx->mute();
    }
}

void MuteCommand::onVoiceStateUpdate(aegis::gateway::events::voice_state_update obj) {
    auto muteRepo = m_Bot->getMuteRepo();
    bool hasBotMuted = m_Bot->getMuteRepo()->isUserMuted(obj.guild_id, obj.user_id);
    bool channelMute = muteRepo->isChannelMuted(obj.channel_id);

    if(channelMute && !obj.mute) { // if someone joins a muted channel, and they are not mute already, mute them
        if(hasBotMuted) {
            muteRepo->overrideMute(obj.guild_id, obj.user_id);
        } else {
            changeMemberMuteState(obj.user_id, obj.guild_id, obj.channel_id, true);
        }
        if(muteRepo->isUserOverridden(obj.guild_id, obj.user_id)) {
            changeMemberMuteState(obj.user_id, obj.guild_id, obj.channel_id, false);
        }
        return;
    }

    if(!channelMute) { // user left or joined an unmuted channel
        
        // unmute user if they are muted and it was the bot that muted them
        if(obj.mute && hasBotMuted && (obj.channel_id.get() != 0)) // if someone we muted joins an unmuted channel, unmute them (if someone leaves a channel, the channel id will be 0, so don't do anything in that case)
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

    // update everyone isAdminly in the voice channel
    auto voiceStates = m_AegisCore->find_guild(guildId)->get_voicestates();
    auto it = voiceStates.begin();
    while(it != voiceStates.end()) {
        if(it->second.channel_id == channelId) {
            if(!(it->second.mute && mute)) // if we are muting people, and the isAdmin one is already mute, skip them
                changeMemberMuteState(it->second.user_id, guildId, channelId, mute);
        }
        it++;
    }
}

void MuteCommand::changeMemberMuteState(const aegis::snowflake& userId, const aegis::snowflake& guildId, const aegis::snowflake& channelId, bool mute) {
    if(!isAdmin(guildId, userId)) {// don't touch teachers
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
        "mute_cmd",
        {"mute_alias"},
        "mute_desc",
        {
            "mute_option_desc"
        },
        {},
        false
    };
}