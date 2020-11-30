#include <cbpch.h>

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

HandRepository::HandRepository() {
    m_Log = ClassroomBot::get().getLog();

    m_DB = ClassroomBot::get().getDatabase();
}

std::list<aegis::snowflake> HandRepository::get(const aegis::snowflake& channelId) {
    auto client = m_DB->requestClient();
    mongocxx::cursor result
        = (*client)[m_DB->dbName()]["Hands"].find(document{} << "channelId" << channelId.gets() << finalize);

    std::list<aegis::snowflake> hands;
    for(auto doc : result) {
        std::string value = doc["userId"].get_string().value.to_string();
        hands.emplace_back(value);
    }

    return hands;
}

void HandRepository::raise(const aegis::snowflake& channelId, const aegis::snowflake& user) {
    auto client = m_DB->requestClient();
    (*client)[m_DB->dbName()]["Hands"].insert_one(document{}
        << "channelId" << channelId.gets()
        << "userId" << user.gets()
        << "updated" << bsoncxx::types::b_date(std::chrono::system_clock::now())
        << finalize
    );
}

void HandRepository::lower(const aegis::snowflake& channelId, const aegis::snowflake& user) {
    auto client = m_DB->requestClient();
    (*client)[m_DB->dbName()]["Hands"].delete_one(document{}
        << "channelId" <<channelId.gets()
        << "userId" << user.gets()
        << finalize
    );
}

void HandRepository::clear(const aegis::snowflake& channelId) {
    auto client = m_DB->requestClient();
    (*client)[m_DB->dbName()]["Hands"].delete_many(document{}
        << "channelId" << channelId.gets()
        << finalize
    );
}

void HandRepository::expire() {
    auto expiryCliff = std::chrono::system_clock::now() - std::chrono::hours(29 * 24); //29 days * 24 hrs/day
    auto client = m_DB->requestClient();
    (*client)[m_DB->dbName()]["Hands"].delete_many(document{}
        << "updated" << open_document
            << "$lt" << bsoncxx::types::b_date(expiryCliff)
        << close_document
        << finalize
    );
}