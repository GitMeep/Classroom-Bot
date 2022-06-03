#include <bot/bot.h>

#include <bot/persistence/repo/muteRepo.h>

#include <cstdint>
#include <iostream>
#include <vector>
#include <bsoncxx/json.hpp>
#include <mongocxx/client.hpp>
#include <mongocxx/stdx.hpp>
#include <mongocxx/uri.hpp>
#include <bsoncxx/builder/stream/helpers.hpp>
#include <bsoncxx/builder/stream/document.hpp>
#include <bsoncxx/builder/stream/array.hpp>

using bsoncxx::builder::stream::close_array;
using bsoncxx::builder::stream::close_document;
using bsoncxx::builder::stream::document;
using bsoncxx::builder::stream::finalize;
using bsoncxx::builder::stream::open_array;
using bsoncxx::builder::stream::open_document;

// static members
Poco::LRUCache<dpp::snowflake, bool> MuteRepo::m_ChannelCache;
Poco::LRUCache<dpp::snowflake, std::set<dpp::snowflake>> MuteRepo::m_UsersCache;
Poco::LRUCache<dpp::snowflake, std::set<dpp::snowflake>> MuteRepo::m_UserOverridesCache;

std::set<dpp::snowflake> MuteRepo::getMutedUsers(const dpp::snowflake& guildId) {
    if(m_UsersCache.has(guildId)) {
        return *(m_UsersCache.get(guildId));
    }
    
    auto client = DB::requestClient();
    auto result = (*client)[DB::name()]["MutedUsers"].find(document{} << "guildId" << std::to_string(guildId) << finalize);

    std::set<dpp::snowflake> users;
    for (auto doc : result) {
        std::string value = std::string(doc["userId"].get_utf8().value);
        std::string decryptedValue = value;
        users.emplace(std::stoull(decryptedValue));
    }

    m_UsersCache.add(guildId, users);

    return users;

}

void MuteRepo::markUser(const dpp::snowflake& guildId, const dpp::snowflake& user, bool muted) {
    if(m_UsersCache.has(guildId)) {
        auto guildMutes = m_UsersCache.get(guildId);
        if(muted) {
            guildMutes->emplace(user);
        } else {
            guildMutes->erase(user);
        }
    }

    auto client = DB::requestClient();
    if(muted) {
        (*client)[DB::name()]["MutedUsers"].insert_one(document{}
            << "guildId" << std::to_string(guildId)
            << "userId" << std::to_string(user)
            << finalize
        );
    } else {
        (*client)[DB::name()]["MutedUsers"].delete_one(document{}
            << "guildId" << std::to_string(guildId)
            << "userId" << std::to_string(user)
            << finalize
        );
    }
    
}

bool MuteRepo::isUserMarked(const dpp::snowflake& guildId, const dpp::snowflake& userId) {
    if(m_UsersCache.has(guildId)) {
        return m_UsersCache.get(guildId)->count(userId) == 1;
    }

    auto client = DB::requestClient();
    auto result =  (*client)[DB::name()]["MutedUsers"].find_one(document{}
        << "guildId" << std::to_string(guildId)
        << "userId" << std::to_string(userId)
        << finalize
    );

    Poco::SharedPtr<std::set<dpp::snowflake>> guildMutes;
    if(m_UsersCache.has(guildId)) {
        guildMutes = m_UsersCache.get(guildId);
    } else {
        guildMutes = new std::set<dpp::snowflake>;
    }
    guildMutes->emplace(userId);
    m_UsersCache.add(guildId, guildMutes);

    return (bool)result;
}

void MuteRepo::markOverride(const dpp::snowflake& guildId, const dpp::snowflake& user, bool overwritten) {
    Poco::SharedPtr<std::set<dpp::snowflake>> overrides;
    if(m_UserOverridesCache.has(guildId)) {
        overrides = m_UserOverridesCache.get(guildId);
    } else {
        overrides = new std::set<dpp::snowflake>;
    }

    if(overwritten) {
        overrides->emplace(user);
    } else {
        overrides->erase(user);
    }

    m_UserOverridesCache.add(guildId, overrides);
}

bool MuteRepo::isUserOverridden(const dpp::snowflake& guildId, const dpp::snowflake& user) {
    Poco::SharedPtr<std::set<dpp::snowflake>> overrides;
    if(m_UserOverridesCache.has(guildId)) {
        overrides = m_UserOverridesCache.get(guildId);
    } else {
        return false;
    }

    return overrides->count(user) == 1; 
}

bool MuteRepo::isChannelMarked(const dpp::snowflake& channelId) {
    if(m_ChannelCache.has(channelId)) {
        return *(m_ChannelCache.get(channelId));
    }

    auto client = DB::requestClient();
    auto result =  (*client)[DB::name()]["MutedChannels"].find_one(document{}
        << "channelId" << std::to_string(channelId)
        << finalize
    );

    m_ChannelCache.add(channelId, (bool)result);

    return (bool)result;
}

void MuteRepo::markChannel(const dpp::snowflake& channelId, bool muted) {
    m_ChannelCache.add(channelId, muted);

    auto client = DB::requestClient();
    if(muted) {
        (*client)[DB::name()]["MutedChannels"].insert_one(document{}
            << "channelId" << std::to_string(channelId)
            << finalize
        );
    } else {
        m_UserOverridesCache.remove(channelId);
        (*client)[DB::name()]["MutedChannels"].delete_one(document{}
            << "channelId" << std::to_string(channelId)
            << finalize
        );
    }
}
