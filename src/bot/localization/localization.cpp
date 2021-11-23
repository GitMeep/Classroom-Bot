#include <nlohmann/json.hpp>

#include <bot/bot.h>

#include <fstream>
#include <filesystem>
namespace fs = std::filesystem;

#include "localization.h"

// load default english string set
Localization::Localization() {
    addLanguage({
        // general
        {"no_dm", "This command can't be used in DM's."},
        {"admin_required", "You must be an admin to use this command."},
        {"above_zero", "Please enter a number above 0."},
        {"unknown_cmd", "Unknown command."},

        // question command
        {"question_cmd", "question"},
        {"question_alias", "q"},
        {"question_desc", "Ask questions that a teacher can then answer one by one."},
        {"question_option_ask", "ask"},
        {"question_option_list", "list"},
        {"question_option_next", "next"},
        {"question_option_clear", "clear"},
        {"question_option_ask_desc", "`ask [question]`: ask a question."},
        {"question_option_list_desc", "`list`: list all questions."},
        {"question_option_next_desc", "`next`: (admin only) show the next question, and remove it from the queue."},
        {"question_option_clear_desc", "`clear`: (admin only) clear the question queue."},
        {"question_usage", "Command usage: `question [ask/list/next/clear]`"},
        {"question_limit", "Queue limit reached (max 50 questions)."},
        {"no_questions", "No questions left."},
        {"empty_question", "You can't ask an empty question."},

        // hand command
        {"hand_cmd", "hand"},
        {"hand_alias", "h"},
        {"hand_desc", "Show of hands. Raise or lower your hand to indicate a question or an answer to one. The hands are stored per-channel."},
        {"hand_option_up", "up"},
        {"hand_option_down", "down"},
        {"hand_option_list", "list"},
        {"hand_option_next", "next"},
        {"hand_option_pick", "pick"},
        {"hand_option_random", "random"},
        {"hand_option_clear", "clear"},
        {"hand_option_up_desc", "`up`: raise your hand."},
        {"hand_option_down_desc", "`down`: lower your hand."},
        {"hand_option_list_desc", "`list`: list all users with their hand raised."},
        {"hand_option_next_desc", "`next`: (admin only) show the next user with a raised hand, and lower it."},
        {"hand_option_pick_desc", "`pick [number]`: (admin only) pick a hand from the list."},
        {"hand_option_random_desc", "`random`: (admin only) pick a random user with their hand raised, and lower it."},
        {"hand_option_clear_desc", "`clear`: (admin only) lower all hands."},
        {"hand_usage", "Command usage: `hand [up/down/next/pick/random/list/clear]`"},
        {"pick_number", "Please enter the number to pick."},
        {"valid_number", "Please enter a valid number."},
        {"reasonable_number", "Please enter a reasonable number."},
        {"already_raised", "You already have your hand raised."},
        {"hand_limit", "Queue limit reached (max 50 hands)."},
        {"no_hands", "No hands are raised."},
        {"hand_oor", "Not that many people have their hand raised."},
        {"hand_up_users", "Users with their hand up:"},

        // mute command
        {"mute_cmd", "mute"},
        {"mute_alias", "m"},
        {"mute_desc", "(Admin only) Toggles mute on the voice channel that you are in. Everyone in a muted channel, except teachers, get server muted. This way you can easily mute a class who wont stop talking."},
        {"mute_option_desc", "`[Channel ID]`: Mute a voice channel that you aren't in. Right click on a voice channel and press `Copy ID` to get it's id."},
        {"not_in_vc", "You are not in a voice channel. To mute a specifc channel, use `mute [channel id]`"},
        {"invalid_id", "Invalid ID"},
        {"vc_not_exist", "That voice channel doesn't exist."},

        // settings command
        {"settings_cmd", "settings"},
        {"settings_alias", "s"},
        {"settings_desc", "Configure the server's settings."},
        {"settings_option_get", "get"},
        {"settings_option_set", "set"},
        {"settings_option_set_desc", "`set [prefix/role] [value]`: set a setting (Admin only)."},
        {"settings_option_get_desc", "`get`: see the settings for your server."},
        {"settings_usage", "Command usage: `settings [get/set] [settings] [value]`.\nSettings are `prefix`, `role` and `language`."},
        {"no_att", "Please enter the role name without @ in front."},
        {"unknown_language", "That language is not supported. Supported languages are:"},
        {"settings_prefix", "Prefix"},
        {"settings_role_name", "Admin role name"},
        {"settings_language", "Language"},
        {"settings_setting_prefix", "prefix"},
        {"settings_setting_role", "role"},
        {"settings_setting_lang", "language"},
        {"settings_for", "Settings for:"},

        // help command
        {"help_cmd", "help"},
        {"help_alias", "he"},
        {"help_desc", "Display this help page. Use `help [command]` to get more information about a specific command."},
        {"help_option_desc", "`[command]`: get help about a specific command."},
        {"help_response_description", "To use the admin only commands, you need a role named \"Teacher\". The name of this role can be changed with the `settings` command. If you have any questions or want to report a bug, join the [support server](https://discord.gg/dqmTAZY). If you like the bot, please upvote it on [top.gg](https://top.gg/bot/691945666896855072) or support development via [Github sponsors.](https://github.com/sponsors/GitMeep)"},
        {"help_response_title", "Commands"},
        {"help_aliases", "Aliases"},
        {"help_options", "Options"},

        // invite command
        {"invite_cmd", "invite"},
        {"invite_desc", "Like the bot and want it on your own server? Use this command to get an invite link in your DM's."},
        {"invite_response", "Invite me to your server using this link: \nhttps://discordapp.com/api/oauth2/authorize?client_id=691945666896855072&permissions=297888850&scope=bot"}

    }, "eng", "English", "");

    // load other languages from ./lang
    if(!fs::exists("./lang")) {
        ClassroomBot::getBot().getLog()->warn("Couldn't find languages directory at ./lang, not loading extra languages.");
        return;
    }
    for(auto& p : fs::directory_iterator("./lang")) {
        std::string path = p.path();
        std::string ext = path.substr(path.length() - 5);
        if(ext == ".json") loadFromFile(path);
    }
}

void Localization::addLanguage(const std::unordered_map<std::string, std::string>& strings, const std::string& code, const std::string& name, const std::string& translator) {
    std::string missingStrings = "";
    auto s = m_Strings["eng"].begin();
    while(s != m_Strings["eng"].end()) {
        if(strings.count(s->first) == 0) {
            missingStrings += s->first + " ";
        }
        s++;
    }

    if(missingStrings != "") {
        ClassroomBot::getBot().getLog()->warn("Language " + code + " is missing strings: " + missingStrings + "\nLoading anyawys.");
    }

    m_Strings[code] = strings;
    m_Languages.emplace_back(code, name);
    m_Translators[code] = translator;
    ClassroomBot::getBot().getLog()->info("Loaded language " + name + " with code " + code + ".");
}



void Localization::loadFromFile(const std::string& path) {
    std::fstream file;
    file.open(path, std::ios::in);

    if(!file.is_open()) {
        ClassroomBot::getBot().getLog()->warn("Could not open language file " + path + " even though it should exist.");
        return;
    } else {
        std::string jsonString;
        file.seekg(0, std::ios::end);
        jsonString.resize(file.tellg());
        file.seekg(0, std::ios::beg);
        file.read(&jsonString[0], jsonString.size());
        file.close();

        try {
            nlohmann::json j = nlohmann::json::parse(jsonString);
            std::string name, code, translator;
            if(j.count("name") == 1) {
                name = j["name"].get<std::string>();
                if(name == "template") return; // do not bother parsing the template file
            } else {
                ClassroomBot::getBot().getLog()->warn("File " + path + " did not include a name, ignoring it.");
                return;
            }
            if(j.count("code") == 1) {
                code = j["code"].get<std::string>();
            } else {
                ClassroomBot::getBot().getLog()->warn("File " + path + " did not include a code, ignoring it.");
                return;
            }
            if(j.count("translator") == 1) {
                translator = j["translator"].get<std::string>();
            }
            if(j.count("strings") == 1 && j["strings"].is_array()) {
                std::unordered_map<std::string, std::string> strings;
                nlohmann::json::iterator it = j["strings"].begin();
                while(it != j["strings"].end()) {
                    strings[it.value().items().begin().key()] = it.value().items().begin().value();
                    it++;
                }
                addLanguage(strings, code, name, translator);
            } else {
                ClassroomBot::getBot().getLog()->warn("File " + path + " did not include and array of strings, ignoring it.");
                return;
            }
        } catch (nlohmann::json::parse_error& e) {
            ClassroomBot::getBot().getLog()->warn("Failed to parse language file: " + path + "\n" + std::string(e.what()));
            return;
        }
    }
}

std::string Localization::getString(const std::string& lang, const std::string& name) {
    if(m_Strings.count(lang) == 0) {
        spdlog::get("classroombot")->warn("Language " + lang + " requested, but not loaded, falling back to english.");
        return getString("eng", name);
    }
    if(m_Strings[lang].count(name) == 0) {
        spdlog::get("classroombot")->warn("String " + name + " requested, but not loaded in language " + lang);
        return "Could not find string " + name;
    }
    
    return m_Strings[lang][name];

}

bool Localization::hasLanguage(const std::string& lang) {
    return m_Strings.count(lang) == 1;
}

std::vector<std::pair<std::string, std::string>> Localization::getLanguages() {
    return m_Languages;
}

std::string Localization::getLanguageName(const std::string& code) {
    auto lang = m_Languages.begin();
    while(lang != m_Languages.end()) {
        if(lang->first == code) return lang->second;
        lang++;
    }
    return "Unknown language";
}

std::string Localization::getTranslator(const std::string& code) {
    if(m_Translators.count(code) == 1) return m_Translators[code];
    return "";
}

LocHelper::LocHelper(const std::shared_ptr<Localization>& loc, const std::string& language)
 : m_Loc(loc) {
     if(language == "") {
         m_Language = "eng";
     } else {
         m_Language = language;
     }
 }

std::string LocHelper::get(const std::string& stringName) {
    return m_Loc->getString(m_Language, stringName);
}
