#include <bot/persistence/hash/sha256.h>
#include <crypto++/base64.h>
#include <crypto++/filters.h>
#include <crypto++/sha.h>

std::string base64Sha256(std::string inputString) {
  std::string      digest;
  CryptoPP::SHA256 sha256;

  CryptoPP::StringSource hash(inputString, true,
    new CryptoPP::HashFilter(sha256,
      new CryptoPP::Base64Encoder(
        new CryptoPP::StringSink(
          digest
        )
      )
    )
  );

  return digest;
}

std::string base64Sha256(uint64_t snowflake) {
  return base64Sha256(std::to_string(snowflake));
}