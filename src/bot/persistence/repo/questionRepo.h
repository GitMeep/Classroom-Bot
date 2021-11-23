#pragma once

#include <bot/persistence/db.h>
#include <bot/persistence/model/question.h>
#include <Poco/LRUCache.h>
class QuestionRepository {
public:
    QuestionRepository();

    std::deque<Question> get(dpp::snowflake channelId);
    void ask(const dpp::snowflake& channelId, const dpp::snowflake& userId, const std::string& question);
    void dismiss(const dpp::snowflake& channelId, const dpp::snowflake& userId);
    void clear(const dpp::snowflake& channelId);

    void expire();

private:
    Poco::LRUCache<dpp::snowflake, std::deque<Question>> m_Cache;

    std::shared_ptr<DB> m_DB;
    std::shared_ptr<Encryption> m_Encryption;
    std::shared_ptr<spdlog::logger> m_Log;
};