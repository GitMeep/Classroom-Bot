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
    
    auto voiceStates = m_Cluster->find_guild(ctx->getGuildId())->get_voicestates();
    if(!voiceStates.count(ctx->getUserId()) && parameters.size() < 1) {
        ctx->respond("not_in_vc");
        return;
    }

    dpp::snowflake voiceChannel;
    if(parameters.size() < 1) {
        voiceChannel = voiceStates[ctx->getUserId()].channel_id;
    } else {
        try {
            long long id = std::stoll(parameters[0]);
            voiceChannel = dpp::snowflake(id);
        } catch (std::invalid_argument& e) {
            ctx->respond("invalid_id");
            return;
        }
        if(
        m_Cluster->find_guild(ctx->getGuildId())->find_channel(voiceChannel) == nullptr ||
        m_Cluster->find_guild(ctx->getGuildId())->find_channel(voiceChannel)->get_type() != aegis::gateway::objects::channel::channel_type::Voice) {
            ctx->respond("vc_not_exist");
            return;
        }
    }
    
    auto muteRepo = m_Bot->getMuteRepo();
    auto guildId = ctx->getGuildId();
    bool mute = true;
    if(muteRepo->isChannelMarked(voiceChannel)) {
        mute = false;
        ctx->unmute();
    } else {
        ctx->mute();
    }
    muteRepo->markChannel(voiceChannel, mute);
    muteAllIn(voiceChannel, guildId, mute);
}

void MuteCommand::muteAllIn(const dpp::snowflake& channelId, const dpp::snowflake& guildId, bool mute) {
    auto muteRepo = m_Bot->getMuteRepo();
    auto voiceStates = m_Cluster->find_guild(guildId)->get_voicestates();
    auto it = voiceStates.begin();
    while(it != voiceStates.end()) {
        if(it->second.channel_id == channelId) {
            auto memberId = it->first;
            if(!isAdmin(guildId, memberId)) { // don't mute and mark someone if they are an admin
                if(!(mute && it->second.mute)) {
                    if(!(!mute && !muteRepo->isUserMarked(guildId, memberId))) {
                        muteAndMark(memberId, channelId, guildId, mute);
                    }
                }
            }
        }
        it++;
    }
}

void MuteCommand::muteAndMark(const dpp::snowflake& userId, const dpp::snowflake& channelId, const dpp::snowflake& guildId, bool mute) {
    m_Cluster->find_guild(guildId)->modify_guild_member(userId, {}, mute, {}, {}, {});

    auto muteRepo = m_Bot->getMuteRepo();
    muteRepo->markUser(guildId, userId, mute);

    if(!mute) {
        muteRepo->markOverride(guildId, userId, mute);
    }
}

void MuteCommand::onVoiceStateUpdate(aegis::gateway::events::voice_state_update obj) {
    auto muteRepo = m_Bot->getMuteRepo();
    bool channelMarked = muteRepo->isChannelMarked(obj.channel_id);
    bool userMarked = muteRepo->isUserMarked(obj.guild_id, obj.user_id);
    bool userMute = obj.mute;
    bool left = obj.channel_id == 0; // user left a channel
    
    if(channelMarked && !userMute && !userMarked) onUnmutedUserInMarkedChannel(obj.user_id, obj.channel_id, obj.guild_id);
    if(userMarked && !channelMarked && !left) onMarkedUserInUnmarkedChannel(obj.user_id, obj.channel_id, obj.guild_id);

    if(userMarked && !userMute) onMarkedUserUnmuted(obj.user_id, obj.channel_id, obj.guild_id);
    if(userMarked && userMute) onMarkedUserMuted(obj.user_id, obj.channel_id, obj.guild_id);
}

void MuteCommand::onUnmutedUserInMarkedChannel(const dpp::snowflake& userId, const dpp::snowflake& channelId, const dpp::snowflake& guildId) {
    auto muteRepo = m_Bot->getMuteRepo();
    if(!(isAdmin(guildId, userId) || muteRepo->isUserOverridden(channelId, userId))) { // if the user is not an admin, and hasn't been overriden, mute them
        muteAndMark(userId, channelId, guildId, true);
    }
}

void MuteCommand::onMarkedUserInUnmarkedChannel(const dpp::snowflake& userId, const dpp::snowflake& channelId,  const dpp::snowflake& guildId) {
    muteAndMark(userId, channelId, guildId, false);
}

void MuteCommand::onMarkedUserUnmuted(const dpp::snowflake& userId, const dpp::snowflake& channelId,  const dpp::snowflake& guildId) {
    auto muteRepo = m_Bot->getMuteRepo();
    muteRepo->markOverride(guildId, userId, true);
}

void MuteCommand::onMarkedUserMuted(const dpp::snowflake& userId, const dpp::snowflake& channelId,  const dpp::snowflake& guildId) {
    auto muteRepo = m_Bot->getMuteRepo();
    muteRepo->markOverride(guildId, userId, false);
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