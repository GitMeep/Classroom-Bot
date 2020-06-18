#include "cbpch.h"
#include "db.h"

using namespace pqxx;
DB::DB(const std::string& connString) {
    m_Log = spdlog::get("classroombot");

    try {
        m_Connection = std::make_unique<connection>(connString);
    } catch (std::exception& e) {
        m_Log->error("Could not connect to database: {} ", e.what());
    }

    m_Log->info("Connected to database");

}

pqxx::result DB::query(const std::string& query) {
    std::lock_guard lock(m_MTX);
    work t(*m_Connection);

    result res = t.exec(query);

    t.commit();

    return res;
}

bool DB::prepare(const std::string& name, const std::string& query) {
    std::lock_guard lock(m_MTX);
    m_Connection->prepare(name, query);
    return true;
}

bool DB::verifyTable(const std::string& name, std::list<std::pair<std::string, std::string>> columns) {
    std::lock_guard lock(m_MTX);
    work t(*m_Connection);

    result res = t.exec("SELECT column_name, data_type FROM information_schema.COLUMNS WHERE TABLE_NAME = '" + t.esc(name) + "';");

    t.commit();

    if(res.size() == 0) {
        m_Log->warn("Table {} not found", name);
        return false;
    }

    if(res.size() != columns.size()) {
        m_Log->warn("Table {} contains {} columns, expected {}.", name, res.size(), columns.size());
        return false;
    }

    auto col = columns.begin();
    while(col != columns.end()) {
        bool found = false;
        auto row = res.begin();
        while(row != res.end()) {
            if(row[0].c_str() == col->first) {
                found = true;
                if(row[1].c_str() != col->second) {
                    m_Log->warn("Column {} in table {} has type {}, expected {}", row[0].c_str(), name, row[1].c_str(), col->second);
                    return false;
                }
            }
            row++;
        }
        if(!found) {
            m_Log->warn("Couldn't find column {} in table {}", col->first, name);
            return false;
        }
        col++;
    }

    return true;

}