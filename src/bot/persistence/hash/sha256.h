#pragma once

#include <string>

std::string base64Sha256(std::string plaintext);
std::string base64Sha256(uint64_t snowflake);