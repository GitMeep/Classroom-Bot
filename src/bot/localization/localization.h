#pragma once

class Localization {
public:
    Localization();
    void loadFromFile(const std::string& fileName);

    std::string getString(const std::string& lang, const std::string& name);

private:
    std::unordered_map<std::string, std::unordered_map<std::string, std::string>> m_Strings; // [lang][name]

};