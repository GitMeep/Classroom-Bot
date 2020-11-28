#pragma once

#include <nlohmann/json.hpp>
#include <spdlog/spdlog.h>

using json = nlohmann::json;

class ClassroomBot;

class Config {
public:
    Config();
    void loadFromFile(const std::string& path);
    json get();
    bool isLoaded();
    json operator[](const std::string& key);
    
private:
    void writeDefaultConfig(const std::string& path);
    void fillEnvVars();
    bool m_Loaded = false;
    json m_Config;
    std::shared_ptr<spdlog::logger> m_Log;

};