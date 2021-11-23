#include <bot/persistence/db.h>
#include <bot/bot.h>
#include <bot/config/config.h>

DB::DB() {
    auto conf = (*ClassroomBot::getBot().getConfig())["persistence"];

    std::string url =    conf["url"];
    m_DBName = conf["db_name"];

    this->encryption = std::make_unique<Encryption>();

    mongocxx::uri uri(url);
    m_Pool = std::make_unique<mongocxx::pool>(uri);
}

std::string DB::dbName() {
    return m_DBName;
}

mongocxx::v_noabi::pool::entry DB::requestClient() {
    return m_Pool->acquire();
}