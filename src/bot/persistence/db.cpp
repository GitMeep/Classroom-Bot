#include <bot/persistence/db.h>
#include <bot/bot.h>
#include <bot/config/config.h>

// static members
mongocxx::instance DB::m_MongoInstance;
std::unique_ptr<mongocxx::pool> DB::m_Pool;
std::string DB::m_DBName;
std::mutex DB::m_MTX;

void DB::init() {
    auto conf = Config::get()["persistence"];

    std::string url = conf["url"];
    m_DBName = conf["db_name"];

    mongocxx::uri uri(url);
    m_Pool = std::make_unique<mongocxx::pool>(uri);
}

std::string DB::name() {
    return m_DBName;
}

mongocxx::v_noabi::pool::entry DB::requestClient() {
    return m_Pool->acquire();
}
