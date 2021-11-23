#pragma once

struct Question {
    dpp::snowflake userId;
    std::string question;

    Question(std::string userId_, std::string question_) {
        userId = dpp::snowflake(userId_);
        question = question_;
    }
};