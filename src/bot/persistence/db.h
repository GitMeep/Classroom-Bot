#pragma once

#include <bot/persistence/encryption/encryption.h>

#include <mongocxx/client.hpp>
#include <mongocxx/pool.hpp>
#include <mongocxx/uri.hpp>
#include <mongocxx/instance.hpp>

class DB {
public:
    DB();

    mongocxx::v_noabi::pool::entry requestClient();
    std::string dbName();

    std::shared_ptr<Encryption> encryption;

private:
    mongocxx::instance m_MongoInstance;
    std::unique_ptr<mongocxx::pool> m_Pool;

    std::string m_DBName;

    std::shared_ptr<spdlog::logger> m_Log;
    std::mutex m_MTX;

};