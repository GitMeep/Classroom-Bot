#include "config.h"
#include "../bot.h"

#include <fstream>
#include <cstdlib>

json defaultConfig = {
    {"bot", {
        {"token", "${BOT_TOKEN}"},
        {"owner", "${OWNER_ID}"},
    }},
    {"persistence", {
        {"enabled", "${ENABLE_PERSISTENCE}"},
        {"url", "${DATABASE_URL}"},
    }}
};

Config::Config() : _log(spdlog::get("aegis")) {}

void Config::loadFromFile(std::string path) {
    _log->info("Loading config from file: " + path);
    std::fstream file;
    file.open(path, std::ios::in);

    if(!file.is_open()) {
        _log->error("Could not open config file, creating default");
        writeDefaultConfig(path);
        _config = defaultConfig;
    } else {
        std::string jsonString;
        file.seekg(0, std::ios::end);
        jsonString.resize(file.tellg());
        file.seekg(0, std::ios::beg);
        file.read(&jsonString[0], jsonString.size());
        file.close();

        try {
            json j = json::parse(jsonString);
            _config = j;
        } catch (json::parse_error& e) {
            _log->info("Failed to parse settings: \n" + std::string(e.what()));
            throw std::runtime_error("Couldn't load settings! To generate a clean config file, delete your existing one and run the program again.");
        }
    }
    fillEnvVars();
    _loaded = true;
}

// check if string is env variable, if it is replace it with the value
std::string replaceVar(std::string val) {
    if(val.size() == 0) return "";
    std::string beginning = val.substr(0,2); // two first chars
    std::string last = val.substr(val.length()-1, 1);

    if(beginning == "${" && last == "}") {
        std::string varName = val.substr(2, val.length()-3);
        char* varVal = getenv(varName.c_str());
        if(varVal == NULL) return "";
        return varVal;
    }
    return val;
}

// recursively traverse json and replace vars
json replaceEnvVars(json& js) {
    json obj;
    for(auto& entry : js) {
        if(entry.is_object()) {
            obj = replaceEnvVars(entry);
        } else {
            if(entry.is_string()) {
                std::string val = entry.get<std::string>();
                entry = replaceVar(val);
            }
        }
    }
    return obj;
}

// searches config for values contained within ${} and replaces it with the environmen variable
void Config::fillEnvVars() {
    replaceEnvVars(_config);
}

json Config::getValue(std::string key) {
    if(_config.count(key) && !_config[key].is_null() && _loaded) {
        return _config[key];
    } else {
        return {};
    }
}

bool Config::isLoaded() {
    return _loaded;
}

void Config::writeDefaultConfig(std::string path) {
    std::fstream file;
    file.open(path, std::ios::out | std::ios::trunc);

    if(!file.is_open()) {
        _log->error("Could not create file.");
        throw std::runtime_error("Could not create config file.");
    }

    file << defaultConfig.dump(4);
    file.close();
}