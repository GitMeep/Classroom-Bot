#include <bot/bot.h>
#include <bot/persistence/db.h>
#include <bot/persistence/hash/sha256.h>
#include <bot/tasks/migratedb.h>

#include <bsoncxx/builder/stream/document.hpp>
#include <bsoncxx/builder/stream/helpers.hpp>
#include <bsoncxx/json.hpp>
#include <cstdint>
#include <mongocxx/client.hpp>
#include <mongocxx/stdx.hpp>
#include <vector>

using bsoncxx::builder::stream::close_array;
using bsoncxx::builder::stream::close_document;
using bsoncxx::builder::stream::document;
using bsoncxx::builder::stream::finalize;
using bsoncxx::builder::stream::open_array;
using bsoncxx::builder::stream::open_document;

void migrateMutedUsers(mongocxx::v_noabi::pool::entry& client) {
  mongocxx::cursor result = (*client)["ClassroomBot"]["MutedUsersOld"].find(document {} << finalize);

  std::vector<bsoncxx::document::value> documents;
  for (auto doc : result) {
    auto guildId = doc["guildId"];
    auto userId  = doc["userId"];

    if (guildId && userId) {
      documents.push_back(
          document {}
          << "guildId" << base64Sha256(std::string(guildId.get_utf8().value))
          << "userId" << std::int64_t(std::stoul(std::string(userId.get_utf8().value)))
          << finalize
      );
    }
  }

  (*client)["ClassroomBot"]["MutedUsers"].insert_many(documents);
}

void migrateMutedChannels(mongocxx::v_noabi::pool::entry& client) {
  mongocxx::cursor result = (*client)["ClassroomBot"]["MutedChannelsOld"].find(document {} << finalize);

  std::vector<bsoncxx::document::value> documents;
  for (auto doc : result) {
    auto channelId = doc["channelId"];

    if (channelId) {
      documents.push_back(
          document {}
          << "guildId" << base64Sha256(std::string(channelId.get_utf8().value))
          << finalize
      );
    }
  }

  (*client)["ClassroomBot"]["MutedChannels"].insert_many(documents);
}

void taskMigratedb(const CommandContext& ctx) {
  LOG_DEBUG(std::string("Running task: ") + std::string(__func__));
  ctx.replyUnlocalized(std::string("Running task: ") + std::string(__func__));

  auto client = DB::requestClient();

  LOG_DEBUG("Migrating muted users");
  migrateMutedUsers(client);

  LOG_DEBUG("Migrating muted channels");
  migrateMutedChannels(client);

  ctx.replyUnlocalizedChannel(std::string("Task done: ") + std::string(__func__));
  LOG_DEBUG(std::string("Task done: ") + std::string(__func__));
}