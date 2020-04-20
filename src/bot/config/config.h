#pragma once

#include <nlohmann/json.hpp>
#include <spdlog/spdlog.h>

using json = nlohmann::json;

class ClassroomBot;

class Config {
public:
    Config();
    void loadFromFile(std::string path);
    json getValue(std::string key);
    bool isLoaded();
    
private:
    void writeDefaultConfig(std::string path);
    void fillEnvVars();
    bool _loaded = false;
    json _config;
    std::shared_ptr<spdlog::logger> _log;

};