#pragma once

#include <vector>

class Localization {
public:
    Localization();
    void loadFromFile(const std::string& fileName);

    void addLanguage(const std::unordered_map<std::string, std::string>& strings, const std::string& code, const std::string& name, const std::string& translator);
    std::string getString(const std::string& lang, const std::string& name);
    bool hasLanguage(const std::string& lang);
    std::vector<std::pair<std::string, std::string>> getLanguages();
    std::string getLanguageName(const std::string& code);
    std::string getTranslator(const std::string& code);

private:
    std::unordered_map<std::string, std::unordered_map<std::string, std::string>> m_Strings; // [lang][name]
    std::vector<std::pair<std::string, std::string>> m_Languages;
    std::unordered_map<std::string, std::string> m_Translators;

};

class LocHelper {
public:
    LocHelper(const std::shared_ptr<Localization>& loc, const std::string& language);
    std::string get(const std::string& stringName);

private:
    std::string m_Language;
    std::shared_ptr<Localization> m_Loc;
};
