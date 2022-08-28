#pragma once

struct Question {
  std::string    questionId;
  dpp::snowflake userId;
  std::string    question;

  Question(std::string _questionId, dpp::snowflake _userId, std::string _question) {
    questionId = _questionId;
    userId     = _userId;
    question   = _question;
  }
};