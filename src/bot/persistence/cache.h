#pragma once

#include "cbpch.h"

namespace aegis {
    class snowflake;
}

template <typename T>
class GuildCache {
public:
    GuildCache(std::chrono::seconds timeout = 500s) : m_Timeout(timeout) {}

    void save(const aegis::snowflake& guildId, const T& value) {
        auto guard = std::lock_guard(m_Mtx);
        m_Values[guildId] = value;
        m_UpdateTimes[guildId] = std::chrono::steady_clock::now();
        m_Mtx.unlock();
    }

    T get(const aegis::snowflake& guildId) {
        auto guard = std::lock_guard(m_Mtx);
        if(m_Values.count(guildId)) {
            return m_Values[guildId];
        } else {
            return T();
        }
    }

    void forget(const aegis::snowflake& guildId) {
        m_Values.erase(guildId);
        m_UpdateTimes.erase(guildId);
    }

    bool has(const aegis::snowflake& guildId) {
        auto guard = std::lock_guard(m_Mtx);
        return m_Values.count(guildId);
    }

    void timeout() {
        auto guard = std::lock_guard(m_Mtx);
        auto now = std::chrono::steady_clock::now();

        auto it = m_UpdateTimes.begin();
        while (it != m_UpdateTimes.end()) {
            auto duration = std::chrono::duration_cast<std::chrono::seconds>(now - it->second);
            if (duration > m_Timeout) {
                m_Values.erase(it->first);
                m_UpdateTimes.erase(it->first);
            }
        }
    }

private:
    std::mutex m_Mtx;
    std::unordered_map<aegis::snowflake, T> m_Values;
    std::unordered_map<aegis::snowflake, std::chrono::time_point<std::chrono::steady_clock>> m_UpdateTimes;
    std::chrono::seconds m_Timeout;
};