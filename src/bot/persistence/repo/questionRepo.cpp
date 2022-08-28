#include <bot/bot.h>
#include <bot/persistence/hash/sha256.h>
#include <bot/persistence/model/question.h>
#include <bot/persistence/repo/questionRepo.h>

#include <bsoncxx/builder/stream/array.hpp>
#include <bsoncxx/builder/stream/document.hpp>
#include <bsoncxx/builder/stream/helpers.hpp>
#include <bsoncxx/json.hpp>
#include <cstdint>
#include <iostream>
#include <mongocxx/client.hpp>
#include <mongocxx/stdx.hpp>
#include <mongocxx/uri.hpp>
#include <vector>

using bsoncxx::builder::stream::close_array;
using bsoncxx::builder::stream::close_document;
using bsoncxx::builder::stream::document;
using bsoncxx::builder::stream::finalize;
using bsoncxx::builder::stream::open_array;
using bsoncxx::builder::stream::open_document;

using mongocxx::pipeline;

Question QuestionRepo::get(const std::string& questionId) {
  auto client = DB::requestClient();

  auto result = (*client)[DB::name()]["Questions"].find_one(document {} << "_id" << bsoncxx::oid(questionId) << finalize);

  if (result) {
    auto view = result.value().view();
    return Question(
        view["_id"].get_oid().value.to_string(),
        view["userId"].get_int64().value,
        std::string(view["question"].get_utf8().value)
    );
  }

  return Question("", 0, "");
}

Question QuestionRepo::getAndDismiss(const std::string& questionId) {
  auto client = DB::requestClient();

  auto result = (*client)[DB::name()]["Questions"].find_one_and_delete(document {} << "_id" << bsoncxx::oid(questionId) << finalize);

  if (result) {
    auto view = result.value().view();
    return Question(
        view["_id"].get_oid().value.to_string(),
        view["userId"].get_int64().value,
        std::string(view["question"].get_utf8().value)
    );
  }

  return Question("", 0, "");
}

std::deque<Question> QuestionRepo::getByChannel(const dpp::snowflake& channelId) {
  auto client = DB::requestClient();

  mongocxx::cursor result = (*client)[DB::name()]["Questions"].find(document {} << "channelId" << base64Sha256(channelId) << finalize);

  std::deque<Question> questions;
  for (auto doc : result) {
    std::string questionId = doc["_id"].get_oid().value.to_string();
    long        userId     = doc["userId"].get_int64().value;
    std::string question   = std::string(doc["question"].get_utf8().value);

    questions.emplace_back(questionId, userId, question);
  }

  return questions;
}

Question QuestionRepo::getNextByChannel(const dpp::snowflake& channelId) {
  auto client = DB::requestClient();

  mongocxx::pipeline p {};
  p.match(document {} << "channelId" << base64Sha256(channelId) << finalize);
  // p.sort(1); // should already be sorted
  p.limit(1);

  mongocxx::cursor result = (*client)[DB::name()]["Questions"].aggregate(p);

  auto begin = result.begin();
  if (result.begin() == result.end()) {
    return Question("", 0, ""); // no documents available
  } else {
    return Question(
        (*begin)["_id"].get_oid().value.to_string(),
        (*begin)["userId"].get_int64().value,
        std::string((*begin)["question"].get_utf8().value)
    );
  }
}

Question QuestionRepo::getNextByChannelAndUser(const dpp::snowflake& channelId, const dpp::snowflake& userId) {
  auto client = DB::requestClient();

  mongocxx::pipeline p {};
  p.match(
      document {}
      << "channelId" << base64Sha256(channelId)
      << "userId" << std::int64_t(userId)
      << finalize
  );
  // p.sort(1); // should already be sorted
  p.limit(1);

  mongocxx::cursor result = (*client)[DB::name()]["Questions"].aggregate(p);

  auto begin = result.begin();
  if (result.begin() == result.end()) {
    return Question("", 0, ""); // no documents available
  } else {
    return Question(
        (*begin)["_id"].get_oid().value.to_string(),
        (*begin)["userId"].get_int64().value,
        std::string((*begin)["question"].get_utf8().value)
    );
  }
}

Question QuestionRepo::getRandomByChannel(const dpp::snowflake& channelId) {
  auto client = DB::requestClient();

  mongocxx::pipeline p {};
  p.match(document {} << "channelId" << base64Sha256(channelId) << finalize);
  p.sample(1);

  mongocxx::cursor result = (*client)[DB::name()]["Questions"].aggregate(p);

  auto begin = result.begin();
  if (result.begin() == result.end()) {
    return Question("", 0, ""); // no documents available
  } else {
    return Question(
        (*begin)["_id"].get_oid().value.to_string(),
        (*begin)["userId"].get_int64().value,
        std::string((*begin)["question"].get_utf8().value)
    );
  }
}

std::deque<Question> QuestionRepo::getByChannelAndUser(const dpp::snowflake& channelId, const dpp::snowflake& userId) {
  auto client = DB::requestClient();

  mongocxx::cursor result = (*client)[DB::name()]["Questions"].find(
      document {} << "$and"
                  << open_array
                  << open_document << "channelId" << base64Sha256(channelId) << close_document
                  << open_document << "userId" << std::int64_t(userId) << close_document
                  << close_array
                  << finalize
  );

  std::deque<Question> questions;
  for (auto doc : result) {
    std::string questionId = doc["_id"].get_oid().value.to_string();
    long        userId     = doc["userId"].get_int64().value;
    std::string question   = std::string(doc["question"].get_utf8().value);

    questions.emplace_back(questionId, userId, question);
  }

  return questions;
}

void QuestionRepo::ask(const dpp::snowflake& channelId, const dpp::snowflake& userId, const std::string& question) {
  auto client = DB::requestClient();
  (*client)[DB::name()]["Questions"].insert_one(
      document {} << "channelId" << base64Sha256(channelId)
                  << "userId" << std::int64_t(userId)
                  << "question" << DB::encrypt(question)
                  << "updated" << bsoncxx::types::b_date(std::chrono::system_clock::now())
                  << finalize
  );
}

void QuestionRepo::dismiss(const std::string& questionId) {
  auto client = DB::requestClient();
  (*client)[DB::name()]["Questions"].delete_one(document {} << "_id" << bsoncxx::oid(questionId) << finalize);
}

void QuestionRepo::clearChannel(const dpp::snowflake& channelId) {
  auto client = DB::requestClient();
  (*client)[DB::name()]["Questions"].delete_many(document {} << "channelId" << base64Sha256(channelId) << finalize);
}

void QuestionRepo::expire() {
  auto expiryCliff = std::chrono::system_clock::now() - std::chrono::hours(29 * 24); // 29 days * 24 hrs/day
  auto client      = DB::requestClient();
  (*client)[DB::name()]["Questions"].delete_many(document {} << "updated" << open_document << "$lt" << bsoncxx::types::b_date(expiryCliff) << close_document << finalize);
}
