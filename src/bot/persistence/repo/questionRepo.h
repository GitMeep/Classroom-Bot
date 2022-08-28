#pragma once

#include <Poco/LRUCache.h>
#include <bot/persistence/db.h>
#include <bot/persistence/model/question.h>
#include <dpp/dpp.h>
class QuestionRepo {
public:
  QuestionRepo(const QuestionRepo&) = delete;

  static std::deque<Question> getByChannel(const dpp::snowflake& channelId);
  static std::deque<Question> getByChannelAndUser(const dpp::snowflake& channelId, const dpp::snowflake& userId);
  static Question             getNextByChannel(const dpp::snowflake& channelId);
  static Question             getNextByChannelAndUser(const dpp::snowflake& channelId, const dpp::snowflake& userId);
  static Question             getRandomByChannel(const dpp::snowflake& channelId);
  static Question             get(const std::string& questionId);
  static Question             getAndDismiss(const std::string& questionId);
  static void                 ask(const dpp::snowflake& channelId, const dpp::snowflake& userId, const std::string& question);
  static void                 dismiss(const std::string& questionId);
  static void                 clearChannel(const dpp::snowflake& channelId);

  static void expire();
};