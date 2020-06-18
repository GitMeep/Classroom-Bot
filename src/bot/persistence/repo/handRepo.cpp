#include "cbpch.h"

#include "bot/persistence/repo/handRepo.h"
#include "bot/persistence/cache.h"

HandRepository::HandRepository(const std::shared_ptr<DB>& db) : m_DB(db) {
    m_Log = spdlog::get("classroombot");

    bool tableValid = m_DB->verifyTable("Hands", {
        {"guildId", "bigint"},
        {"userId", "bigint"},
        {"raisedWhen", "timestamp without time zone"}
    });

    if (!tableValid) {
        m_Log->warn("Hands table invalid, won't save settings to database");
        m_InvalidTable = true;
    }

    m_DB->prepare("getHands", "SELECT \"userId\" FROM \"Hands\" WHERE \"guildId\" = $1 ORDER BY \"raisedWhen\" ASC;");
    m_DB->prepare("raiseHand", "INSERT INTO \"Hands\" VALUES ($1, $2, current_timestamp);");
    m_DB->prepare("lowerHand", "DELETE FROM \"Hands\" WHERE \"guildId\" = $1 AND \"userId\" = $2");
    m_DB->prepare("clearHands", "DELETE FROM \"Hands\" WHERE \"guildId\" = $1");
}

std::list<aegis::snowflake> HandRepository::get(const aegis::snowflake& guildId) {
    if(m_Cache.has(guildId) || m_InvalidTable) {
        return m_Cache.get(guildId);
    }

    auto res = m_DB->execPrep("getHands", guildId.gets());

    std::list<aegis::snowflake> hands;

    auto it = res.begin();
    while (it != res.end()) {
        hands.push_back({it[0].as<long>()});
        it++;
    }

    m_Cache.save(guildId, hands);

    return hands;
}

void HandRepository::raise(const aegis::snowflake& guildId, const aegis::snowflake& user) {
    if(!m_InvalidTable) {
        m_DB->execPrep("raiseHand", guildId.gets(), user.gets());
    }

    auto hands = m_Cache.get(guildId);
    hands.push_back(user);
    m_Cache.save(guildId, hands);
}

void HandRepository::lower(const aegis::snowflake& guildId, const aegis::snowflake& user) {
    if(!m_InvalidTable) {
        m_DB->execPrep("lowerHand", guildId.gets(), user.gets());
    }

    auto hands = m_Cache.get(guildId);
    hands.erase(std::find(hands.begin(), hands.end(), user));
    if(!hands.size()) {
        m_Cache.forget(guildId);
    } else {
        m_Cache.save(guildId, hands);
    }
}

void HandRepository::clear(const aegis::snowflake& guildId) {
    if(!m_InvalidTable) {
        m_DB->execPrep("clearHands", guildId.gets());
    }

    m_Cache.forget(guildId);
}