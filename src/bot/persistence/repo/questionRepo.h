#pragma once

#include <bot/persistence/db.h>
#include <bot/persistence/model/question.h>

class QuestionRepository {
public:
    QuestionRepository();

    std::deque<Question> get(aegis::snowflake channelId);
    void ask(const aegis::snowflake& channelId, const aegis::snowflake& userId, const std::string& question);
    void dismiss(const aegis::snowflake& channelId, const aegis::snowflake& userId);
    void clear(const aegis::snowflake& channelId);

    void expire();

private:
    std::shared_ptr<DB> m_DB;
    std::shared_ptr<Encryption> m_Encryption;
    std::shared_ptr<spdlog::logger> m_Log;
};