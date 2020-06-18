#pragma once

class DB {
public:
    DB(const std::string& connString);

    pqxx::result query(const std::string& query);
    bool prepare(const std::string& name, const std::string& query);

    template <typename... Args>
    pqxx::result execPrep(const std::string& name, Args... args) {
        std::lock_guard lock(m_MTX);
        pqxx::work t(*m_Connection);
        pqxx::result res = t.exec_prepared(name, args...);
        t.commit();
        return res;
    }

    bool verifyTable(const std::string& name, std::list<std::pair<std::string, std::string>> columns);

private:
    std::unique_ptr<pqxx::connection> m_Connection;
    std::shared_ptr<spdlog::logger> m_Log;
    std::mutex m_MTX;

};