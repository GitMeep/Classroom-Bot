#include <dpp/nlohmann/json.hpp>

#include <bot/bot.h>

#include <fstream>
#include <filesystem>
namespace fs = std::filesystem;

#include "localization.h"

std::unordered_map<std::string, std::unordered_map<std::string, std::string>> Localization::m_Strings;
std::vector<std::pair<std::string, std::string>> Localization::m_Languages;
std::unordered_map<std::string, std::string> Localization::m_Translators;

// load default english string set
void Localization::init() {
    addLanguage({
        // general
        {"no_dm", "This command can't be used in DM's."},
        {"admin_required", "You must be an admin to use this command."},
        {"above_zero", "Please enter a number above 0."},
        {"unknown_cmd", "Unknown command."},

        // question
        {"question_cmd", "question"},
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

        // hand
        {"hand_cmd_raise", "raise"},
            {"hand_cmd_raise_desc", "Raise your hand."},

        {"hand_cmd_lower", "lower"},
            {"hand_cmd_lower_desc", "Lower your hand."},
        
        {"hand_cmd_hands", "hands"},
            {"hand_cmd_hands_desc", "List everyone with their hand raised."},

        {"hand_cmd_pick", "pick"},
            {"hand_cmd_pick_desc", "Various ways to pick a hand."},
            {"hand_cmd_pick_option_next", "next"},
                {"hand_cmd_pick_option_next_desc", "Pick the next hand in the queue."},
            {"hand_cmd_pick_option_list", "list"},
                {"hand_cmd_pick_option_list_desc", "Pick a hand from a list."},
            {"hand_cmd_pick_option_user", "user"},
                {"hand_cmd_pick_option_user_desc", "Pick a specific user."},
            {"hand_cmd_pick_option_random", "random"},
                {"hand_cmd_pick_option_random_desc", "Pick a random hand in the queue."},
            {"hand_cmd_pick_option_clear", "clear"},
                {"hand_cmd_pick_option_clear_desc", "Lower all hands."},

        {"hand_already_raised", "You already have your hand raised."},
        {"hand_cleared_hands", "{clearer} cleared all hands"},
        {"hand_limit", "Queue limit reached (max 50 hands)."},
        {"hand_no_hands", "No hands are raised."},
        {"hand_up_users", "Users with their hand up:"},
        {"hand_picked_message", "{picker} picked {picked}'s hand"},
        {"hand_picked_message_not_raised", "{picker} picked {picked}'s hand, even though it wasn't raised"},
        {"hand_user_raised_hand", "{raiser} raised their hand"},

        // attendance
        {"attendance_cmd_attendance", "attendance"},
            {"attendance_cmd_attendance_desc", "Take attendance."},
        
        {"attendance_taking_attendance", "{teacher} is taking attendance."},
        {"attendance_people_present", "People present"},
        {"attendance_already_indicated", "You have already indicated attendance"},
        {"attendance_present_button", "I'm present"},

        // mute
        {"mute_cmd", "mute"},
        {"mute_desc", "(Admin only) Toggles mute on the voice channel that you are in. Everyone in a muted channel, except teachers, get server muted. This way you can easily mute a class who wont stop talking."},
        {"mute_option_desc", "`[Channel ID]`: Mute a voice channel that you aren't in. Right click on a voice channel and press `Copy ID` to get it's id."},
        {"not_in_vc", "You are not in a voice channel. To mute a specifc channel, use `mute [channel id]`"},
        {"invalid_id", "Invalid ID"},
        {"vc_not_exist", "That voice channel doesn't exist."},

        // settings
        {"settings_cmd", "settings"},
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

        // help
        {"help_cmd", "help"},
        {"help_desc", "Display this help page. Use `help [command]` to get more information about a specific command."},
        {"help_option_desc", "`[command]`: get help about a specific command."},
        {"help_response_description", "To use the admin only commands, you need a role named \"Teacher\". The name of this role can be changed with the `settings` command. If you have any questions or want to report a bug, join the [support server](https://discord.gg/dqmTAZY). If you like the bot, please upvote it on [top.gg](https://top.gg/bot/691945666896855072) or support development via [Github sponsors.](https://github.com/sponsors/GitMeep)"},
        {"help_response_title", "Commands"},
        {"help_aliases", "Aliases"},
        {"help_options", "Options"},

        // invite
        {"invite_cmd_invite", "invite"},
            {"invite_cmd_invite_desc", "Like the bot and want it on your own server? Use this command to get an invite link in your DM's."},
            {"invite_label", "Invite me"}

    }, "en-US", "English", "");

    // load other languages from ./lang
    if(!fs::exists("./lang")) {
        LOG_WARN("Couldn't find languages directory at ./lang, not loading extra languages.");
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
    auto s = m_Strings["en-US"].begin();
    while(s != m_Strings["en-US"].end()) {
        if(strings.count(s->first) == 0) {
            missingStrings += s->first + " ";
        }
        s++;
    }

    if(missingStrings != "") {
        LOG_WARN("Language " + code + " is missing strings: " + missingStrings + "\nLoading anyawys.");
    }

    m_Strings[code] = strings;
    m_Languages.emplace_back(code, name);
    m_Translators[code] = translator;
    LOG_INFO("Loaded language " + name + " with code " + code + ".");
}



void Localization::loadFromFile(const std::string& path) {
    std::fstream file;
    file.open(path, std::ios::in);

    if(!file.is_open()) {
        LOG_WARN("Could not open language file " + path + " even though it should exist.");
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
                LOG_WARN("File " + path + " did not include a name, ignoring it.");
                return;
            }
            if(j.count("code") == 1) {
                code = j["code"].get<std::string>();
            } else {
                LOG_WARN("File " + path + " did not include a code, ignoring it.");
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
                LOG_WARN("File " + path + " did not include and array of strings, ignoring it.");
                return;
            }
        } catch (nlohmann::json::parse_error& e) {
            LOG_WARN("Failed to parse language file: " + path + "\n" + std::string(e.what()));
            return;
        }
    }
}

const bool Localization::hasString(const std::string& name, const std::string& lang) {
    return m_Strings[lang].count(name) == 1;
}

static const std::string emptyString = "";
const std::string& Localization::getString(const std::string& name, const std::string& lang) {
    if(lang == "") return getString(name, "en-US");

    if(m_Strings.count(lang) == 0) {
        LOG_WARN("Language " + lang + " requested, but not loaded, falling back to english.");
        return getString(name, "en-US");
    }

    if(m_Strings[lang].count(name) == 0) {
        LOG_WARN("String " + name + " requested, but not loaded in language " + lang);
        return emptyString;
    }
    
    return m_Strings[lang][name];
}

bool Localization::hasLanguage(const std::string& lang) {
    return m_Strings.count(lang) == 1;
}

const std::vector<std::pair<std::string, std::string>>& Localization::getLanguages() {
    return m_Languages;
}

static const std::string unknownLanguage = "Unknown language";
const std::string& Localization::getLanguageName(const std::string& code) {
    auto lang = m_Languages.begin();
    while(lang != m_Languages.end()) {
        if(lang->first == code) return lang->second;
        lang++;
    }
    return unknownLanguage;
}

const std::string& Localization::getTranslator(const std::string& code) {
    if(m_Translators.count(code) == 1) return m_Translators[code];
    return emptyString;
}
