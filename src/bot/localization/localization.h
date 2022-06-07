#pragma once

#include <string>
#include <vector>
#include <functional>

class Localization {
public:
    typedef std::function<void(const std::string&)> StringAcceptor;

    Localization(const Localization&) = delete;

    static void init();
    static void loadFromFile(const std::string& fileName);

    static void addLanguage(const std::unordered_map<std::string, std::string>& strings, const std::string& code, const std::string& name, const std::string& translator);
    static bool hasLanguage(const std::string& lang);
    static const std::vector<std::pair<std::string, std::string>>& getLanguages();

    static const bool hasString(const std::string& name, const std::string& lang = "");
    static const std::string& getString(const std::string& name, const std::string& lang = "");
    static void doIfHasString(const std::string& name, const std::string& lang, StringAcceptor handler);
    static const std::string& getLanguageName(const std::string& code);
    static const std::string& getTranslator(const std::string& code);

private:
    static std::unordered_map<std::string, std::unordered_map<std::string, std::string>> m_Strings; // [lang][name]
    static std::vector<std::pair<std::string, std::string>> m_Languages;
    static std::unordered_map<std::string, std::string> m_Translators;
};
