#include <cbpch.h>

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

QuestionRepository::QuestionRepository() {
    m_Log = ClassroomBot::get().getLog();

    m_DB = ClassroomBot::get().getDatabase();
    this->m_Encryption = m_DB->encryption;
}

std::deque<Question> QuestionRepository::get(aegis::snowflake channelId) {
    if(m_Cache.has(channelId)) {
        return *(m_Cache.get(channelId));
    }

    auto client = m_DB->requestClient();
    mongocxx::cursor result
        = (*client)[m_DB->dbName()]["Questions"].find(document{} << "channelId" << channelId.gets() << finalize);

    std::deque<Question> questions;
    for(auto doc : result) {
        std::string userId = doc["userId"].get_utf8().value.to_string();
        std::string question = doc["question"].get_utf8().value.to_string();
        std::string decryptedQuestion = m_Encryption->decrypt(question);

        questions.emplace_back(userId, decryptedQuestion);
    }

    m_Cache.add(channelId, questions);

    return questions;
}

void QuestionRepository::ask(const aegis::snowflake& channelId, const aegis::snowflake& userId, const std::string& question) {
    if(m_Cache.has(channelId)) {
        m_Cache.get(channelId)->emplace_back(userId.gets(), question);
    }
    
    auto client = m_DB->requestClient();
    (*client)[m_DB->dbName()]["Questions"].insert_one(document{}
        << "channelId" << channelId.gets()
        << "userId" << userId.gets()
        << "question" <<  m_Encryption->encrypt(question)
        << "updated" << bsoncxx::types::b_date(std::chrono::system_clock::now())
        << finalize
    );
}

void QuestionRepository::dismiss(const aegis::snowflake& channelId, const aegis::snowflake& userId) {
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

    auto client = m_DB->requestClient();
    (*client)[m_DB->dbName()]["Questions"].delete_one(document{}
        << "channelId" << channelId.gets()
        << "userId" << userId.gets()
        << finalize
    );
}

void QuestionRepository::clear(const aegis::snowflake& channelId) {
    m_Cache.remove(channelId);

    auto client = m_DB->requestClient();
    (*client)[m_DB->dbName()]["Questions"].delete_many(document{}
        << "channelId" << channelId.gets()
        << finalize
    );
}

void QuestionRepository::expire() {
    auto expiryCliff = std::chrono::system_clock::now() - std::chrono::hours(29 * 24); //29 days * 24 hrs/day
    auto client = m_DB->requestClient();
    (*client)[m_DB->dbName()]["Questions"].delete_many(document{}
        << "updated" << open_document
            << "$lt" << bsoncxx::types::b_date(expiryCliff)
        << close_document
        << finalize
    );
}
