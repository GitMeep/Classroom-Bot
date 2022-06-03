#pragma once

#include <bot/persistence/encryption/encryption.h>

#include <mongocxx/client.hpp>
#include <mongocxx/pool.hpp>
#include <mongocxx/uri.hpp>
#include <mongocxx/instance.hpp>

class DB {
public:
    DB(const DB&) = delete;

    static void init();

    static mongocxx::v_noabi::pool::entry requestClient();
    static std::string name();

private:
    static mongocxx::instance m_MongoInstance;
    static std::unique_ptr<mongocxx::pool> m_Pool;

    static std::string m_DBName;

    static std::mutex m_MTX;
};
