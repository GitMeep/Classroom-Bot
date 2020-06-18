#include "cbpch.h"

#include "questionRepo.h"
#include "bot/persistence/model/question.h"

QuestionRepository::QuestionRepository(const std::shared_ptr<DB>& db) : m_DB(db) {
    m_Log = spdlog::get("classroombot");

    bool tableValid = m_DB->verifyTable("Questions", {
        {"guildId", "bigint"},
        {"userId", "bigint"},
        {"question", "text"},
        {"askedWhen", "timestamp without time zone"}
    });

    if (!tableValid) {
        m_Log->warn("Questions table invalid, won't save settings to database");
        m_InvalidTable = true;
    }

    m_DB->prepare("getQuestions", "SELECT \"guildId\", \"userId\", \"question\" FROM \"Questions\" WHERE \"guildId\" = $1 ORDER BY \"askedWhen\" ASC;");
    m_DB->prepare("askQuestion", "INSERT INTO \"Questions\" VALUES ($1, $2, $3, current_timestamp);");
    m_DB->prepare("dismissQuestion", "DELETE FROM \"Questions\" WHERE \"guildId\" = $1 AND \"userId\" = $2");
    m_DB->prepare("clearQuestions", "DELETE FROM \"Questions\" WHERE \"guildId\" = $1");
}

std::deque<Question> QuestionRepository::get(aegis::snowflake guildId) {
    if(m_Cache.has(guildId) || m_InvalidTable) {
        return m_Cache.get(guildId);
    }

    auto res = m_DB->execPrep("getQuestions", guildId.gets());

    std::deque<Question> questions;

    auto it = res.begin();
    while (it != res.end()) {
        questions.push_back({
            it[0].as<long>(), // guild id
            it[1].as<long>(), // user id
            it[2].c_str()     // question
        });
        it++;
    }

    m_Cache.save(guildId, questions);

    return questions;
}

void QuestionRepository::ask(const aegis::snowflake& guildId, const aegis::snowflake& userId, const std::string& question) {
    if(!m_InvalidTable) {
        m_DB->execPrep("askQuestion", guildId.gets(), userId.gets(), question);
    }

    auto questions = m_Cache.get(guildId);
    questions.push_back({guildId, userId, question});
    m_Cache.save(guildId, questions);
}

void QuestionRepository::dismiss(const aegis::snowflake& guildId, const aegis::snowflake& userId) {
    if(!m_InvalidTable) {
        m_DB->execPrep("dismissQuestion", guildId.gets(), userId.gets());
    }

    auto questions = m_Cache.get(guildId);
    auto it = questions.begin();
    while(it != questions.end()) {
        if(it->guildId == guildId && it->userId == userId) {
            break;
        }
        it++;
    }
    questions.erase(it);
    if (!questions.size()) {
        m_Cache.forget(guildId);
    } else {
        m_Cache.save(guildId, questions);
    }
}

void QuestionRepository::clear(const aegis::snowflake& guildId) {
    if(!m_InvalidTable) {
        m_DB->execPrep("clearQuestions", guildId.gets());
    }

    m_Cache.forget(guildId);
}