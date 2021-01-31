#include <cbpch.h>

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

MuteRepository::MuteRepository() {
    m_Log = ClassroomBot::get().getLog();
    m_DB = ClassroomBot::get().getDatabase();
}

std::set<aegis::snowflake> MuteRepository::getMutedUsers(const aegis::snowflake& guildId) {
    if(m_UsersCache.has(guildId)) {
        return *(m_UsersCache.get(guildId));
    }
    
    auto client = m_DB->requestClient();
    auto result = (*client)[m_DB->dbName()]["MutedUsers"].find(document{} << "guildId" << guildId.gets() << finalize);

    std::set<aegis::snowflake> users;
    for (auto doc : result) {
        std::string value = doc["userId"].get_utf8().value.to_string();
        std::string decryptedValue = value;
        users.emplace(decryptedValue);
    }

    m_UsersCache.add(guildId, users);

    return users;

}

void MuteRepository::muteUser(const aegis::snowflake& guildId, const aegis::snowflake& user) {
    if(m_UsersCache.has(guildId)) {
        m_UsersCache.get(guildId)->emplace(user);
    }

    auto client = m_DB->requestClient();
    auto result = (*client)[m_DB->dbName()]["MutedUsers"].insert_one(document{}
        << "guildId" << guildId.gets()
        << "userId" << user.gets()
        << finalize
    );
}

void MuteRepository::unmuteUser(const aegis::snowflake& guildId, const aegis::snowflake& user) {
    if(m_UsersCache.has(guildId)) {
        auto list = m_UsersCache.get(guildId);
        list->erase(user);
        if(list->size() == 0) {
            m_UsersCache.remove(guildId);
        }
    }

    auto client = m_DB->requestClient();
    (*client)[m_DB->dbName()]["MutedUsers"].delete_one(document{}
        << "guildId" << guildId.gets()
        << "userId" << user.gets()
        << finalize
    );
}

bool MuteRepository::isChannelMuted(const aegis::snowflake& channelId) {
    if(m_ChannelCache.has(channelId)) {
        return *(m_ChannelCache.get(channelId));
    }

    auto client = m_DB->requestClient();
    auto result =  (*client)[m_DB->dbName()]["MutedChannels"].find_one(document{}
        << "channelId" << channelId.gets()
        << finalize
    );

    m_ChannelCache.add(channelId, (bool)result);

    return (bool)result;
}

void MuteRepository::muteChannel(const aegis::snowflake& channelId) {
    m_ChannelCache.add(channelId, true);

    auto client = m_DB->requestClient();
    auto result =  (*client)[m_DB->dbName()]["MutedChannels"].insert_one(document{}
        << "channelId" << channelId.gets()
        << finalize
    );
}

void MuteRepository::unmuteChannel(const aegis::snowflake& channelId) {
    m_ChannelCache.add(channelId, false);

    auto client = m_DB->requestClient();
    auto result =  (*client)[m_DB->dbName()]["MutedChannels"].delete_one(document{}
        << "channelId" << channelId.gets()
        << finalize
    );
}