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
    Encryption(const Encryption&) = delete;

    static void init();

    static std::string encryptOLD(const std::string& plainText);
    static std::string decryptOLD(const std::string& cipherText);

    static std::string encrypt(const std::string& plainText);
    static std::string decrypt(const std::string& cipherText);

    static void generateIV(byte* iv, int size = AES::BLOCKSIZE);

private:
    static AutoSeededRandomPool m_prng;

    static GCM<AES>::Encryption m_Encryption;
    static GCM<AES>::Decryption m_Decryption;

    static SecByteBlock m_Key;
};