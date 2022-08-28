#pragma once

#include <string>
#include <vector>
#include <set>

class Localization {
public:
    Localization(const Localization&) = delete;

    static void init();
    static void loadFromFile(const std::string& fileName);

    static void addLanguage(const std::unordered_map<std::string, std::string>& strings, const std::string& code, const std::string& name, const std::string& translator);
    static bool hasLanguage(const std::string& lang);
    static bool isPartial(const std::string& lang);
    static const std::vector<std::pair<std::string, std::string>>& getLanguages();

    static const bool hasString(const std::string& name, const std::string& lang = "");
    static const std::string& getString(const std::string& name, const std::string& lang = "");
    static const std::string& getLanguageName(const std::string& code);
    static const std::string& getTranslator(const std::string& code);

private:
    static std::unordered_map<std::string, std::unordered_map<std::string, std::string>> m_Strings; // language code -> (string name -> localized string)
    static std::vector<std::pair<std::string, std::string>> m_Languages; // pair<language code, language name>
    static std::unordered_map<std::string, std::string> m_Translators; // language code -> translator
    static std::set<std::string> m_PartialLanguages; // language code -> translator
};
