#include <bot/persistence/encryption/encryption.h>

#include <bot/bot.h>
#include <bot/config/config.h>

#include <crypto++/hex.h>

const int TAG_SIZE = 12;

AutoSeededRandomPool Encryption::m_prng;

GCM<AES>::Encryption Encryption::m_Encryption;
GCM<AES>::Decryption Encryption::m_Decryption;

SecByteBlock Encryption::m_Key;

void Encryption::init() {
    auto conf = Config::get()["persistence"];

    std::string encodedKey = conf["encryption_key"];

    std::string key;

    StringSource ssk(encodedKey, true /*pumpAll*/,
        new HexDecoder(
            new StringSink(key)
        ) // HexDecoder
    ); // StringSource

    m_Key = SecByteBlock((byte*)key.data(), key.size());
}

void Encryption::generateIV(byte* iv, int size) {
    m_prng.Reseed();
    m_prng.GenerateBlock(iv, size);
}

std::string Encryption::encrypt(const std::string& plainText) {
    byte iv[AES::DEFAULT_KEYLENGTH];
    generateIV(iv);
    m_Encryption.SetKeyWithIV(m_Key, m_Key.size(), iv, AES::DEFAULT_KEYLENGTH);

    std::string encodedCipherText;

    try {
        StringSource ss1(plainText, true, 
            new AuthenticatedEncryptionFilter(m_Encryption,
                new HexEncoder(
                    new StringSink(encodedCipherText)
                ), false, TAG_SIZE
            )
        );
    } catch (Exception& e) {
        return "";
    }

    std::string encodedIV;
    StringSource ss2(iv, sizeof(iv), true,
        new HexEncoder(
            new StringSink(encodedIV)
        ) // HexDecoder
    ); // StringSource

    encodedCipherText += encodedIV;

    return encodedCipherText;
}

std::string Encryption::decrypt(const std::string& encodedCipherText) {
    if(encodedCipherText.length() < AES::DEFAULT_KEYLENGTH*2) return "";

    std::string encodedIV = encodedCipherText.substr(encodedCipherText.length()-AES::DEFAULT_KEYLENGTH*2);
    std::string strippedCipherText = encodedCipherText.substr(0, encodedCipherText.length()-AES::DEFAULT_KEYLENGTH*2);
    std::string iv;
    
    StringSource ssv(encodedIV, true /*pumpAll*/,
        new HexDecoder(
            new StringSink(iv)
        ) // HexDecoder
    ); // StringSource

    m_Decryption.SetKeyWithIV(m_Key, m_Key.size(), (byte*)iv.data(), iv.size());

    std::string cipherText;
    StringSource ssk(strippedCipherText, true /*pumpAll*/,
        new HexDecoder(
            new StringSink(cipherText)
        ) // HexDecoder
    ); // StringSource

    std::string plaintext;

    try {
        AuthenticatedDecryptionFilter df(m_Decryption,
            new StringSink(plaintext),
            16U, TAG_SIZE
        );

        StringSource ss2(cipherText, true,
            new Redirector(df)
        );

        if(df.GetLastResult() == true) {
            return plaintext;
        }
    } catch (Exception& e) {
        return "";
    }

    return "";
}