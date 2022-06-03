#pragma once

#include <string>
#include <vector>

class Localization {
public:
    Localization(const Localization&) = delete;

    static void init();
    static void loadFromFile(const std::string& fileName);

    static void addLanguage(const std::unordered_map<std::string, std::string>& strings, const std::string& code, const std::string& name, const std::string& translator);
    static std::string getString(const std::string& lang, const std::string& name);
    static bool hasLanguage(const std::string& lang);
    static std::vector<std::pair<std::string, std::string>> getLanguages();
    static std::string getLanguageName(const std::string& code);
    static std::string getTranslator(const std::string& code);

private:
    static std::unordered_map<std::string, std::unordered_map<std::string, std::string>> m_Strings; // [lang][name]
    static std::vector<std::pair<std::string, std::string>> m_Languages;
    static std::unordered_map<std::string, std::string> m_Translators;
};

class LocHelper {
public:
    LocHelper(const std::string& language);
    std::string get(const std::string& stringName);

private:
    std::string m_Language;
};
