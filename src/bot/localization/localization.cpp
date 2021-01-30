#include <cbpch.h>

#include "localization.h"

// load default english string set
Localization::Localization() {
    m_Strings["eng"] = {
        // general
        {"no_dm", "This command can't be used in DM's."},
        {"admin_required", "You must be an admin to use this command."},
        {"above_zero", "Please enter a number above 0."},

        // question command
        {"question_usage", "Command usage: `question [ask/list/next/clear]`"},
        {"question_limit", "Queue limit reached (max 50 questions)."},
        {"no_questions", "No questions left."},

        // hand command
        {"hand_usage", "Command usage: `hand [up/down/next/pick/random/list/clear]`"},
        {"pick_number", "Please enter the number to pick."},
        {"valid_number", "Please enter a valid number."},
        {"reasonable_number", "Please enter a reasonable number."},
        {"already_raised", "You already have your hand raised."},
        {"hand_limit", "Queue limit reached (max 50 hands)"},
        {"no_hands", "No hands are raised."},
        {"hand_oor", "Not that many people have their hand raised."},
        {"hand_up_users", "Users with their hands up"},

        // help command
        {"unkown_cmd", "Unknown command."},

        // mute command
        {"not_in_vc", "You are not in a voice channel. To mute a specifc channel, use `mute [channel id]`"},
        {"invalid_id", "Invalid ID"},
        {"vc_not_exist", "That voice channel doesn't exist"},

        // pubchem command
        {"comp_not_found", "Couldn't find that compound. Try entering a CID or name."},

        // settings command
        {"settings_usage", "Command usage: settings [get/set] [settings] [value].\nSettings are `prefix` and `role`"},
        {"no_att", "Please enter the role name without @ in front"}

    };
}

void Localization::loadFromFile(const std::string& fileName) {
    // TODO
}

std::string Localization::getString(const std::string& lang, const std::string& name) {
    if(m_Strings.count(lang) == 0) {
        spdlog::get("classroombot")->info("Language " + lang + " requested, but not loaded.");
        return "Could not find language " + lang;
    }
    if(m_Strings[lang].count(name) == 0) {
        spdlog::get("classroombot")->info("String " + name + " requested, but not loaded in language " + lang);
        return "Could not find string " + name;
    }
    
    return m_Strings[lang][name];

}