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
    m_Log = ClassroomBot::getBot().getLog();
    m_DB = ClassroomBot::getBot().getDatabase();
}

std::set<dpp::snowflake> MuteRepository::getMutedUsers(const dpp::snowflake& guildId) {
    if(m_UsersCache.has(guildId)) {
        return *(m_UsersCache.get(guildId));
    }
    
    auto client = m_DB->requestClient();
    auto result = (*client)[m_DB->dbName()]["MutedUsers"].find(document{} << "guildId" << guildId.gets() << finalize);

    std::set<dpp::snowflake> users;
    for (auto doc : result) {
        std::string value = doc["userId"].get_utf8().value.to_string();
        std::string decryptedValue = value;
        users.emplace(decryptedValue);
    }

    m_UsersCache.add(guildId, users);

    return users;

}

void MuteRepository::markUser(const dpp::snowflake& guildId, const dpp::snowflake& user, bool muted) {
    if(m_UsersCache.has(guildId)) {
        auto guildMutes = m_UsersCache.get(guildId);
        if(muted) {
            guildMutes->emplace(user);
        } else {
            guildMutes->erase(user);
        }
    }

    auto client = m_DB->requestClient();
    if(muted) {
        (*client)[m_DB->dbName()]["MutedUsers"].insert_one(document{}
            << "guildId" << guildId.gets()
            << "userId" << user.gets()
            << finalize
        );
    } else {
        (*client)[m_DB->dbName()]["MutedUsers"].delete_one(document{}
            << "guildId" << guildId.gets()
            << "userId" << user.gets()
            << finalize
        );
    }
    
}

bool MuteRepository::isUserMarked(const dpp::snowflake& guildId, const dpp::snowflake& userId) {
    if(m_UsersCache.has(guildId)) {
        return m_UsersCache.get(guildId)->count(userId) == 1;
    }

    auto client = m_DB->requestClient();
    auto result =  (*client)[m_DB->dbName()]["MutedUsers"].find_one(document{}
        << "guildId" << guildId.gets()
        << "userId" << userId.gets()
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

void MuteRepository::markOverride(const dpp::snowflake& guildId, const dpp::snowflake& user, bool overwritten) {
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

bool MuteRepository::isUserOverridden(const dpp::snowflake& guildId, const dpp::snowflake& user) {
    Poco::SharedPtr<std::set<dpp::snowflake>> overrides;
    if(m_UserOverridesCache.has(guildId)) {
        overrides = m_UserOverridesCache.get(guildId);
    } else {
        return false;
    }

    return overrides->count(user) == 1; 
}

bool MuteRepository::isChannelMarked(const dpp::snowflake& channelId) {
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

void MuteRepository::markChannel(const dpp::snowflake& channelId, bool muted) {
    m_ChannelCache.add(channelId, muted);

    auto client = m_DB->requestClient();
    if(muted) {
        (*client)[m_DB->dbName()]["MutedChannels"].insert_one(document{}
            << "channelId" << channelId.gets()
            << finalize
        );
    } else {
        m_UserOverridesCache.remove(channelId);
        (*client)[m_DB->dbName()]["MutedChannels"].delete_one(document{}
            << "channelId" << channelId.gets()
            << finalize
        );
    }
}
