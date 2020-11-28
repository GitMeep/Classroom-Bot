#pragma once
#include <string>

#include <crypto++/aes.h>
#include <crypto++/osrng.h>
#include <crypto++/gcm.h>
#include <crypto++/modes.h>
#include <crypto++/filters.h>

using namespace CryptoPP;

class Encryption {
public:
    Encryption();

    std::string encryptOLD(const std::string& plainText);
    std::string decryptOLD(const std::string& cipherText);

    std::string encrypt(const std::string& plainText);
    std::string decrypt(const std::string& cipherText);

    void generateIV(byte* iv, int size = AES::BLOCKSIZE);

private:
    AutoSeededRandomPool m_prng;

    GCM<AES>::Encryption m_Encryption;
    GCM<AES>::Decryption m_Decryption;

    SecByteBlock m_Key;
};