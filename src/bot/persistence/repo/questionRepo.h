#pragma once

#include "bot/persistence/db.h"
#include "bot/persistence/model/question.h"

class QuestionRepository {
public:
    QuestionRepository(const std::shared_ptr<DB>& db);

    std::deque<Question> get(aegis::snowflake guildId);
    void ask(const aegis::snowflake& guildId, const aegis::snowflake& userId, const std::string& question);
    void dismiss(const aegis::snowflake& guildId, const aegis::snowflake& userId);
    void clear(const aegis::snowflake& guildId);

private:
    std::shared_ptr<DB> m_DB;
    std::shared_ptr<spdlog::logger> m_Log;
    GuildCache<std::deque<Question>> m_Cache;

    bool m_InvalidTable = false;
};