#include <bot/bot.h>
#include <bot/persistence/repo/handRepo.h>

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

#include <chrono>

using bsoncxx::builder::stream::close_array;
using bsoncxx::builder::stream::close_document;
using bsoncxx::builder::stream::document;
using bsoncxx::builder::stream::finalize;
using bsoncxx::builder::stream::open_array;
using bsoncxx::builder::stream::open_document;

Poco::LRUCache<dpp::snowflake, std::list<dpp::snowflake>> HandRepo::m_Cache;

std::list<dpp::snowflake> HandRepo::get(const dpp::snowflake& channelId) {
    if(m_Cache.has(channelId)) {
        return *(m_Cache.get(channelId));
    }

    auto client = DB::requestClient();
    mongocxx::cursor result
        = (*client)[DB::name()]["Hands"].find(document{} << "channelId" << std::to_string(channelId) << finalize);

    std::list<dpp::snowflake> hands;
    for(auto doc : result) {
        std::string value = std::string(doc["userId"].get_utf8().value);
        hands.emplace_back(std::stoull(value));
    }

    m_Cache.add(channelId, hands);

    return hands;
}

void HandRepo::raise(const dpp::snowflake& channelId, const dpp::snowflake& user) {
    if(m_Cache.has(channelId)) {
        auto list = m_Cache.get(channelId);
        list->emplace_back(user);
    } else {
        m_Cache.add(channelId, {user});
    }

    auto client = DB::requestClient();
    (*client)[DB::name()]["Hands"].insert_one(document{}
        << "channelId" << std::to_string(channelId)
        << "userId" << std::to_string(user)
        << "updated" << bsoncxx::types::b_date(std::chrono::system_clock::now())
        << finalize
    );
}

void HandRepo::lower(const dpp::snowflake& channelId, const dpp::snowflake& user) {
    if(m_Cache.has(channelId)) {
        auto list = m_Cache.get(channelId);
        list->remove(user);
        if(list->size() == 0) {
            m_Cache.remove(channelId);
        }
    }

    auto client = DB::requestClient();
    (*client)[DB::name()]["Hands"].delete_one(document{}
        << "channelId" << std::to_string(channelId)
        << "userId" << std::to_string(user)
        << finalize
    );
}

void HandRepo::clear(const dpp::snowflake& channelId) {
    m_Cache.remove(channelId);

    auto client = DB::requestClient();
    (*client)[DB::name()]["Hands"].delete_many(document{}
        << "channelId" << std::to_string(channelId)
        << finalize
    );
}

void HandRepo::expire() {
    auto expiryCliff = std::chrono::system_clock::now() - std::chrono::hours(29 * 24); //29 days * 24 hrs/day
    auto client = DB::requestClient();
    (*client)[DB::name()]["Hands"].delete_many(document{}
        << "updated" << open_document
            << "$lt" << bsoncxx::types::b_date(expiryCliff)
        << close_document
        << finalize
    );
}