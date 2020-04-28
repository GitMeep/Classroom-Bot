#include "handsCommand.h"
#include "../utils/utils.h"

#include <stdlib.h>
#include <time.h>

#define ADMIN_ROLE "Teacher"

const std::string actionMsg = "Please enter a valid action! Options are: ```up, down, next, pick, random, list, clear```";

void HandsCommand::call(std::vector<std::string> parameters, CurrentCommand current) {
    std::lock_guard<std::mutex> guard(_handsMtx);

    Command::call(parameters, current);

    if(parameters.size() == 0) {
        _aegisCore->create_message(_current.channelId, actionMsg);
        return;
    }

    std::string verb = parameters[0];
    if(verb == "up") {
        up();
    }

    else if(verb == "down") {
        down();
    }

    else if(verb == "list") {
        list();
    }

    else if(verb == "next") {
        if(!isTeacher(_current.guildId, _current.userId, _aegisCore, _bot->_settingsRepo)) {
            _aegisCore->create_message(_current.channelId, "You must have the admin role to use this command.");
        } else {
            next();
        }
    }

    else if(verb == "random") {
        if(!isTeacher(_current.guildId, _current.userId, _aegisCore, _bot->_settingsRepo)) {
            _aegisCore->create_message(_current.channelId, "You must have the admin role to use this command.");
        } else {
            random();
        }
    }

    else if(verb == "clear") {
        if(!isTeacher(_current.guildId, _current.userId, _aegisCore, _bot->_settingsRepo)) {
            _aegisCore->create_message(_current.channelId, "You must have the admin role to use this command.");
        } else {
            clear();
        }
    } else if(verb == "pick") {
        if(!isTeacher(_current.guildId, _current.userId, _aegisCore, _bot->_settingsRepo)) {
            _aegisCore->create_message(_current.channelId, "You must have the admin role to use this command.");
        } else {
            int number;
            try {
                number = std::stoi(parameters[1]);
            } catch(std::invalid_argument) {
                _aegisCore->create_message(_current.channelId, "Please enter a valid number");
            } catch(std::out_of_range) {
                _aegisCore->create_message(_current.channelId, "Please enter a reasonable number");
            }
            pick(number);
        }
    }

    else {
        _aegisCore->create_message(_current.channelId, actionMsg);
    }
}

void HandsCommand::up() {
    for(aegis::snowflake handUser : _hands[_current.guildId]) {
        if(_current.userId == handUser) {
            _aegisCore->create_message(_current.channelId, "You already have your hand raised.");
            return;
        }
    }

    _hands[_current.guildId].emplace_back(_current.userId);
    _aegisCore->find_channel(_current.channelId)->create_reaction(aegis::create_reaction_t().message_id(_current.messageId).emoji_text("%E2%9C%85"));
}

void HandsCommand::down() {
    _hands[_current.guildId].remove(_current.userId);

    if(_hands[_current.guildId].size() == 0) {
        _hands.erase(_current.guildId);
    }
    _aegisCore->find_channel(_current.channelId)->create_reaction(aegis::create_reaction_t().message_id(_current.messageId).emoji_text("%E2%9C%85"));
}

void HandsCommand::next() {
    if(_hands[_current.guildId].size() == 0) {
        _aegisCore->create_message(_current.channelId, "No hands up");
        return;
    }
    aegis::snowflake user = _hands[_current.guildId].front();
    std::string username = getUsername(user, _current.guildId, _aegisCore);
    _hands[_current.guildId].pop_front();
    
    if(_hands[_current.guildId].size() == 0) {
        _hands.erase(_current.guildId);
    }

    _aegisCore->create_message(_current.channelId, "```" + username + "```");
}

void HandsCommand::clear() {
    _hands[_current.guildId].clear();
    _hands.erase(_current.guildId);
    _aegisCore->find_channel(_current.channelId)->create_reaction(aegis::create_reaction_t().message_id(_current.messageId).emoji_text("%E2%9C%85"));
}

void HandsCommand::list() {
    if(!_hands.count(_current.guildId)) {
        _aegisCore->create_message(_current.channelId, "No hands up");
        return;
    }
    std::stringstream ss;
    ss << "Users with their hand up:```";

    int number = 1;
    auto it = _hands[_current.guildId].begin();
    while(it != _hands[_current.guildId].end()) {
        ss << number << ": " << getUsername(*it, _current.guildId, _aegisCore) << "\n";
        it++;
        number++;
    }
    ss << "```\n";
    _aegisCore->create_message(_current.channelId, ss.str());
}

void HandsCommand::random() {
    if(_hands[_current.guildId].size() == 0) {
        _aegisCore->create_message(_current.channelId, "No hands up");
        return;
    }
    srand(time(NULL));
    int random = rand() % _hands[_current.guildId].size(); // random number between 0 and size of guild hands

    auto it = _hands[_current.guildId].begin();
    while(random) { // go to random element in guild hands
        it++;
        random--;
    }

    aegis::snowflake user = *it;
    std::string username = getUsername(user, _current.guildId, _aegisCore);
    _hands[_current.guildId].remove(user);

    if(_hands[_current.guildId].size() == 0) {
        _hands.erase(_current.guildId);
    }

    _aegisCore->create_message(_current.channelId, "```" + username + "```");
}

void HandsCommand::pick(int number) {
    if(!_hands.count(_current.guildId)) {
        _aegisCore->create_message(_current.channelId, "No one has their hand raised.");
        return;
    }

    if(number > _hands[_current.guildId].size()) {
        _aegisCore->create_message(_current.channelId, "There aren't that many users with their hands raised");
        return;
    }

    if(number < 1) {
        _aegisCore->create_message(_current.channelId, "Please enter a number above 0.");
        return;
    }

    number--; // start at 0
    auto it = _hands[_current.guildId].begin();
    while(number) {
        it++;
        number--;
    }

    aegis::snowflake user = *it;
    std::string username = getUsername(user, _current.guildId, _aegisCore);
    _hands[_current.guildId].remove(user);

    if(_hands[_current.guildId].size() == 0) {
        _hands.erase(_current.guildId);
    }

    _aegisCore->create_message(_current.channelId, "```" + username + "```");
}

bool HandsCommand::checkPermissions(aegis::permission channelPermissions) {
    return
    channelPermissions.can_add_reactions();
}

CommandInfo HandsCommand::getCommandInfo() {
    return {
        {"hand", "h"},
        "Show of hands",
        {
            "up: raise your hand",
            "down: lower your hand",
            "list: list all users with their hand raised",
            "next: (admin only) show the next user with a raised hand, and lower it",
            "pick [number]: (admin only) pick a user from the list",
            "random: (admin only) pick a random user with their hand raised, and lower it",
            "clear: (admin only) lower all hands"
        },
        "I need permission to add reactions to use this command."
    };
}