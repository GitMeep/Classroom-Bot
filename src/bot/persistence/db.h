#pragma once

#include <bsoncxx/types/bson_value/make_value.hpp>
#include <mongocxx/client.hpp>
#include <mongocxx/client_encryption.hpp>
#include <mongocxx/instance.hpp>
#include <mongocxx/pool.hpp>
#include <mongocxx/uri.hpp>
#include <mutex>

class DB {
public:
  DB(const DB&) = delete;

  static void init();

  static mongocxx::v_noabi::pool::entry requestClient();
  static std::string                    name();
  template <class T>
  static bsoncxx::types::bson_value::value encrypt(T value) {
    return encryptBsonValue(bsoncxx::types::bson_value::make_value(value));
  }

private:
  static mongocxx::instance                           m_MongoInstance;
  static std::unique_ptr<mongocxx::pool>              m_Pool;
  static std::unique_ptr<mongocxx::client_encryption> m_Encryption;

  static bsoncxx::types::bson_value::value encryptBsonValue(bsoncxx::types::bson_value::view value);

  static std::string m_DBName;

  static std::mutex m_MTX;
};
