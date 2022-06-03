#pragma once

#include <dpp/snowflake.h>

#include <bot/persistence/db.h>
#include <bot/persistence/model/question.h>
#include <Poco/LRUCache.h>
class QuestionRepo {
public:
    QuestionRepo(const QuestionRepo&) = delete;

    static std::deque<Question> get(dpp::snowflake channelId);
    static void ask(const dpp::snowflake& channelId, const dpp::snowflake& userId, const std::string& question);
    static void dismiss(const dpp::snowflake& channelId, const dpp::snowflake& userId);
    static void clear(const dpp::snowflake& channelId);

    static void expire();

private:
    static Poco::LRUCache<dpp::snowflake, std::deque<Question>> m_Cache;
};