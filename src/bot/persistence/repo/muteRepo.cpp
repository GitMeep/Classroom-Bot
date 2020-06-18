#include "cbpch.h"

#include "bot/persistence/repo/muteRepo.h"
#include "bot/persistence/cache.h"

MuteRepository::MuteRepository(const std::shared_ptr<DB>& db) : m_DB(db) {
    m_Log = spdlog::get("classroombot");

    bool tableValid = m_DB->verifyTable("MutedUsers", {
        {"guildId", "bigint"},
        {"userId", "bigint"}
    });

    if (!tableValid) {
        m_Log->warn("MutedUsers table invalid, won't save settings to database");
        m_InvalidUserTable = true;
    }

    tableValid = m_DB->verifyTable("MutedChannels", {
        {"channelId", "bigint"}
    });

    if (!tableValid) {
        m_Log->warn("MutedChannels table invalid, won't save settings to database");
        m_InvalidChannelTable = true;
    }

    if (!tableValid) return;

    m_DB->prepare("getMutedUsers", "SELECT \"userId\" FROM \"MutedUsers\" WHERE \"guildId\" = $1;");
    m_DB->prepare("muteUser", "INSERT INTO \"MutedUsers\" VALUES ($1, $2);");
    m_DB->prepare("unmuteUser", "DELETE FROM \"MutedUsers\" WHERE \"guildId\" = $1 AND \"userId\" = $2");

    m_DB->prepare("isChannelMuted", "SELECT EXISTS(SELECT 1 FROM \"MutedChannels\" WHERE \"channelId\" = $1);");
    m_DB->prepare("muteChannel", "INSERT INTO \"MutedChannels\" VALUES ($1);");
    m_DB->prepare("unmuteChannel", "DELETE FROM \"MutedChannels\" WHERE \"channelId\" = $1");
}

std::set<aegis::snowflake> MuteRepository::getMutedUsers(const aegis::snowflake& guildId) {
    if(m_UserCache.has(guildId) || m_InvalidUserTable) {
        return m_UserCache.get(guildId);
    }

    auto res = m_DB->execPrep("getMutedUsers", guildId.gets());
    std::set<aegis::snowflake> mutes;
    auto it = res.begin();
    while (it != res.end()) {
        mutes.insert(it[0].as<long>());
        it++;
    }
    m_UserCache.save(guildId, mutes);
    
    return mutes;
}

void MuteRepository::muteUser(const aegis::snowflake& guildId, const aegis::snowflake& user) {
    if(!m_InvalidUserTable) {
        m_DB->execPrep("muteUser", guildId.gets(), user.gets());
    }

    auto mutes = m_UserCache.get(guildId);
    mutes.insert(user);
    m_UserCache.save(guildId, mutes);
}

void MuteRepository::unmuteUser(const aegis::snowflake& guildId, const aegis::snowflake& user) {
    m_DB->execPrep("unmuteUser", guildId.gets(), user.gets());

    auto mutes = m_UserCache.get(guildId);
    mutes.erase(std::find(mutes.begin(), mutes.end(), user));
    if(!mutes.size()) {
        m_UserCache.forget(guildId);
    } else {
        m_UserCache.save(guildId, mutes);
    }
}

bool MuteRepository::isChannelMuted(const aegis::snowflake& channelId) {
    if (m_MutedChannels.count(channelId)) {
        return true;
    }

    if (!m_InvalidChannelTable) {
        auto res = m_DB->execPrep("isChannelMuted", channelId.gets());
        bool muted = res[0][0].as<bool>();
        if (muted) {
            m_MutedChannels.insert(channelId);
            return true;
        }
    }

    return false;
}

void MuteRepository::muteChannel(const aegis::snowflake& channelId) {
    if(!m_InvalidChannelTable) {
        m_DB->execPrep("muteChannel", channelId.gets());
    }

    m_MutedChannels.insert(channelId);
}

void MuteRepository::unmuteChannel(const aegis::snowflake& channelId) {
    if(!m_InvalidChannelTable) {
        m_DB->execPrep("unmuteChannel", channelId.gets());
    }

    m_MutedChannels.erase(channelId);
}
