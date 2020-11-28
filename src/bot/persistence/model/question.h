#pragma once

#include <cbpch.h>

struct Question {
    aegis::snowflake userId;
    std::string question;

    Question(std::string userId_, std::string question_) {
        userId = aegis::snowflake(userId_);
        question = question_;
    }
};