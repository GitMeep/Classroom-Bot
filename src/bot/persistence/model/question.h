#pragma once

struct Question {
    dpp::snowflake userId;
    std::string question;

    Question(dpp::snowflake _userId, std::string _question) {
        userId = _userId;
        question = _question;
    }
};