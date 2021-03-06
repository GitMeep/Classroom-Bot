#include <bot/config/config.h>
#include <bot/bot.h>

#include <fstream>
#include <cstdlib>

json defaultConfig = {
    {"bot", {
        {"token", "${BOT_TOKEN}"},
        {"owner", "${OWNER_ID}"}
    }},
    {"persistence", {
        {"url", "${MONGO_URL}"},
        {"db_name", "${DATABASE_NAME}"},
        {"encryption_key", "${ENCRYPTION_KEY}"}
    },
    {"topgg", {
        {"enable", "${ENABLE_TOPGG}"},
        {"bot_id", "${TOPGG_ID}"},
        {"token", "${TOPGG_TOKEN}"}
    }}
    }
};

Config::Config() : m_Log(ClassroomBot::get().getLog()) {}

void Config::loadFromFile(const std::string& path) {
    m_Log->info("Loading config from file: " + path);
    std::fstream file;
    file.open(path, std::ios::in);

    if(!file.is_open()) {
        m_Log->warn("Could not open config file, using default config.");
        m_Config = defaultConfig;
    } else {
        std::string jsonString;
        file.seekg(0, std::ios::end);
        jsonString.resize(file.tellg());
        file.seekg(0, std::ios::beg);
        file.read(&jsonString[0], jsonString.size());
        file.close();

        try {
            json j = json::parse(jsonString);
            m_Config = j;
        } catch (json::parse_error& e) {
            m_Log->warn("Failed to parse settings: \n" + std::string(e.what()));
            throw std::runtime_error("Couldn't load settings! To generate a clean config file, delete your existing one and run the program again.");
        }
    }
    fillEnvVars();
    m_Loaded = true;
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
    replaceEnvVars(m_Config);
}

json Config::operator[](const std::string& key) {
    return m_Config[key];
}

json Config::get() {
    return m_Config;
}

bool Config::isLoaded() {
    return m_Loaded;
}