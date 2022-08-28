#include <bot/bot.h>
#include <bot/config/config.h>
#include <bot/persistence/db.h>
#include <crypto++/filters.h>
#include <crypto++/hex.h>

#include <bsoncxx/builder/stream/document.hpp>
#include <mongocxx/exception/operation_exception.hpp>
#include <mongocxx/options/data_key.hpp>

using bsoncxx::builder::basic::kvp;
using bsoncxx::builder::basic::make_document;

using bsoncxx::builder::stream::close_document;
using bsoncxx::builder::stream::document;
using bsoncxx::builder::stream::finalize;
using bsoncxx::builder::stream::open_document;

#define MASTER_KEY_SIZE 96

// static members
mongocxx::instance                           DB::m_MongoInstance;
std::unique_ptr<mongocxx::pool>              DB::m_Pool;
std::unique_ptr<mongocxx::client_encryption> DB::m_Encryption;
std::string                                  DB::m_DBName;
std::mutex                                   DB::m_MTX;

void DB::init() {
  auto conf = Config::get()["persistence"];

  std::string url = conf["url"];
  m_DBName        = conf["db_name"];
  std::string mk  = conf["master_key"];

  static uint8_t key_storage[MASTER_KEY_SIZE];

  CryptoPP::StringSource hash(mk, true, new CryptoPP::HexDecoder(new CryptoPP::ArraySink(key_storage, sizeof(key_storage))));

  bsoncxx::types::b_binary local_master_key {bsoncxx::binary_sub_type::k_binary, sizeof(key_storage), (const uint8_t*)&key_storage};

  auto kms_providers = document {}
                       << "local"
                       << open_document
                       << "key" << local_master_key
                       << close_document
                       << finalize;

  mongocxx::options::auto_encryption auto_encrypt_opts {};
  auto_encrypt_opts.bypass_auto_encryption(true);
  auto_encrypt_opts.kms_providers(kms_providers.view());
  auto_encrypt_opts.key_vault_namespace({"keyvault", "datakeys"});

  mongocxx::options::client client_opts;
  client_opts.auto_encryption_opts(std::move(auto_encrypt_opts));

  mongocxx::uri uri(url);
  m_Pool = std::make_unique<mongocxx::pool>(uri, client_opts);

  auto keyVaultClient = m_Pool->acquire();

  mongocxx::options::client_encryption client_encryption_opts {};
  client_encryption_opts
      .key_vault_client(&(*keyVaultClient))
      .key_vault_namespace({"keyvault", "datakeys"})
      .kms_providers(kms_providers.view());

  m_Encryption = std::make_unique<mongocxx::client_encryption>(client_encryption_opts);

  mongocxx::v_noabi::options::data_key data_key_opts {};
  data_key_opts.key_alt_names({"Key 1"});
  try {
    m_Encryption->create_data_key("local", data_key_opts);
  } catch (mongocxx::operation_exception& e) {
    // don't care
  }
}

std::string DB::name() {
  return m_DBName;
}

mongocxx::pool::entry DB::requestClient() {
  return m_Pool->acquire();
}

bsoncxx::types::bson_value::value DB::encryptBsonValue(bsoncxx::types::bson_value::view value) {
  mongocxx::options::encrypt encrypt_opt {};
  encrypt_opt.algorithm(mongocxx::options::encrypt::encryption_algorithm::k_deterministic);
  encrypt_opt.key_alt_name("Key 1");
  return m_Encryption->encrypt(value, encrypt_opt);
}
