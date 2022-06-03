#include <bot/bot.h>
#include <bot/persistence/repo/questionRepo.h>
#include <bot/persistence/model/question.h>

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

Poco::LRUCache<dpp::snowflake, std::deque<Question>> QuestionRepo::m_Cache;

std::deque<Question> QuestionRepo::get(dpp::snowflake channelId) {
    if(m_Cache.has(channelId)) {
        return *(m_Cache.get(channelId));
    }

    auto client = DB::requestClient();
    mongocxx::cursor result
        = (*client)[DB::name()]["Questions"].find(document{} << "channelId" << std::to_string(channelId) << finalize);

    std::deque<Question> questions;
    for(auto doc : result) {
        std::string userId = std::string(doc["userId"].get_utf8().value);
        std::string question = std::string(doc["question"].get_utf8().value);
        std::string decryptedQuestion = Encryption::decrypt(question);

        questions.emplace_back(std::stoull(userId), decryptedQuestion);
    }

    m_Cache.add(channelId, questions);

    return questions;
}

void QuestionRepo::ask(const dpp::snowflake& channelId, const dpp::snowflake& userId, const std::string& question) {
    if(m_Cache.has(channelId)) {
        m_Cache.get(channelId)->emplace_back(userId, question);
    }
    
    auto client = DB::requestClient();
    (*client)[DB::name()]["Questions"].insert_one(document{}
        << "channelId" << std::to_string(channelId)
        << "userId" << std::to_string(userId)
        << "question" <<  Encryption::encrypt(question)
        << "updated" << bsoncxx::types::b_date(std::chrono::system_clock::now())
        << finalize
    );
}

void QuestionRepo::dismiss(const dpp::snowflake& channelId, const dpp::snowflake& userId) {
    if(m_Cache.has(channelId)) {
        auto deque = m_Cache.get(channelId);

        auto it = deque->begin();
        while(it != deque->end()) {
            if(it->userId == userId) break;
            it++;
        }
        deque->erase(it);

        if(deque->size() == 0) {
            m_Cache.remove(channelId);
        }
    }

    auto client = DB::requestClient();
    (*client)[DB::name()]["Questions"].delete_one(document{}
        << "channelId" << std::to_string(channelId)
        << "userId" << std::to_string(userId)
        << finalize
    );
}

void QuestionRepo::clear(const dpp::snowflake& channelId) {
    m_Cache.remove(channelId);

    auto client = DB::requestClient();
    (*client)[DB::name()]["Questions"].delete_many(document{}
        << "channelId" << std::to_string(channelId)
        << finalize
    );
}

void QuestionRepo::expire() {
    auto expiryCliff = std::chrono::system_clock::now() - std::chrono::hours(29 * 24); //29 days * 24 hrs/day
    auto client = DB::requestClient();
    (*client)[DB::name()]["Questions"].delete_many(document{}
        << "updated" << open_document
            << "$lt" << bsoncxx::types::b_date(expiryCliff)
        << close_document
        << finalize
    );
}
