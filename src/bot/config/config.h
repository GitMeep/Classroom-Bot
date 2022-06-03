#pragma once

#include <dpp/nlohmann/json.hpp>

using json = nlohmann::json;

class ClassroomBot;

class Config {
public:
    Config(const Config&) = delete; // delete any constructor, purely static class

    static void loadFromFile(const std::string& path);
    static json get();
    static bool isLoaded();
    
private:
    static void writeDefaultConfig(const std::string& path);
    static void fillEnvVars();
    static bool m_Loaded;
    static json m_Config;
};