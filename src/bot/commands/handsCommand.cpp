#include "cbpch.h"

#include "handsCommand.h"
#include "../utils/utils.h"
#include "../bot.h"
#include "../persistence/repo/handRepo.h"

const std::string actionMsg = "Please enter a valid action! Options are: ```up, down, next, pick, random, list, clear```";

void HandsCommand::call(const std::vector<std::string>& parameters, MessageInfo* current) {
    
    if(current->isDm) {
        _aegisCore->create_dm_message(current->userId, "Command not supported in DM's");
        return;
    }

    if(parameters.size() == 0) {
        _aegisCore->create_message(current->channelId, actionMsg);
        return;
    }

    std::string verb = parameters[0];
    if(verb == "up") {
        up(current);
    }

    else if(verb == "down") {
        down(current);
    }

    else if(verb == "list") {
        list(current);
    }

    else if(verb == "next") {
        if(!isTeacher(current->guildId, current->userId, _aegisCore, _bot->_settingsRepo)) {
            _aegisCore->create_message(current->channelId, "You must have the admin role to use this command.");
        } else {
            next(current);
        }
    }

    else if(verb == "random") {
        if(!isTeacher(current->guildId, current->userId, _aegisCore, _bot->_settingsRepo)) {
            _aegisCore->create_message(current->channelId, "You must have the admin role to use this command.");
        } else {
            random(current);
        }
    }

    else if(verb == "clear") {
        if(!isTeacher(current->guildId, current->userId, _aegisCore, _bot->_settingsRepo)) {
            _aegisCore->create_message(current->channelId, "You must have the admin role to use this command.");
        } else {
            clear(current);
        }
    } else if(verb == "pick") {
        if(!isTeacher(current->guildId, current->userId, _aegisCore, _bot->_settingsRepo)) {
            _aegisCore->create_message(current->channelId, "You must have the admin role to use this command.");
        } else {
            int number;
            try {
                number = std::stoi(parameters[1]);
            } catch(std::invalid_argument) {
                _aegisCore->create_message(current->channelId, "Please enter a valid number");
            } catch(std::out_of_range) {
                _aegisCore->create_message(current->channelId, "Please enter a reasonable number");
            }
            pick(current, number);
        }
    }

    else {
        _aegisCore->create_message(current->channelId, actionMsg);
    }
}

void HandsCommand::up(MessageInfo* current) {
    auto hands = _bot->_handRepo->get(current->guildId);

    for(auto hand : hands) {
        auto handUser = hand;
        if(current->userId.get() == handUser) {
            _aegisCore->create_message(current->channelId, "You already have your hand raised.");
            return;
        }
    }

    _bot->_handRepo->raise(current->guildId, current->userId);

    _aegisCore->find_channel(current->channelId)->create_reaction(aegis::create_reaction_t().message_id(current->messageId).emoji_text("%E2%9C%85"));
}

void HandsCommand::down(MessageInfo* current) {
    _bot->_handRepo->lower(current->guildId, current->userId);

    _aegisCore->find_channel(current->channelId)->create_reaction(aegis::create_reaction_t().message_id(current->messageId).emoji_text("%E2%9C%85"));
}

void HandsCommand::next(MessageInfo* current) {
    auto hands = _bot->_handRepo->get(current->guildId);

    if(!hands.size()) {
        _aegisCore->create_message(current->channelId, "No hands up");
        return;
    }

    aegis::snowflake user = hands.front();
    std::string username = getUsername(user, current->guildId, _aegisCore);

    _bot->_handRepo->lower(current->guildId, user);

    _aegisCore->create_message(current->channelId, "```" + username + "```");
}

void HandsCommand::clear(MessageInfo* current) {
    _bot->_handRepo->clear(current->guildId);

    _aegisCore->find_channel(current->channelId)->create_reaction(aegis::create_reaction_t().message_id(current->messageId).emoji_text("%E2%9C%85"));
}

void HandsCommand::list(MessageInfo* current) {
    auto hands = _bot->_handRepo->get(current->guildId);

    if(!hands.size()) {
        _aegisCore->create_message(current->channelId, "No hands up");
        return;
    }
    std::stringstream ss;
    ss << "Users with their hand up:```";

    int number = 1;
    auto it = hands.begin();
    while (it != hands.end()) {
        ss << number << ": " << getUsername(*it, current->guildId, _aegisCore) << "\n";
        it++;
        number++;
    }
    ss << "```\n";
    _aegisCore->create_message(current->channelId, ss.str());
}

void HandsCommand::random(MessageInfo* current) {
    auto hands = _bot->_handRepo->get(current->guildId);

    if(!hands.size()) {
        _aegisCore->create_message(current->channelId, "No hands up");
        return;
    }

    std::srand(std::time(NULL));
    int random = std::rand() % hands.size(); // random number between 0 and size of hands

    auto it = hands.begin();
    while(random) { // go to random element in hands
        it++;
        random--;
    }

    aegis::snowflake user = *it;
    std::string username = getUsername(user, current->guildId, _aegisCore);

    _bot->_handRepo->lower(current->guildId, user);

    _aegisCore->create_message(current->channelId, "```" + username + "```");
}

void HandsCommand::pick(MessageInfo* current, int number) {
    auto hands = _bot->_handRepo->get(current->guildId);

    if(!hands.size()) {
        _aegisCore->create_message(current->channelId, "No one has their hand raised.");
        return;
    }

    if(number > hands.size()) {
        _aegisCore->create_message(current->channelId, "There aren't that many users with their hands raised");
        return;
    }

    if(number < 1) {
        _aegisCore->create_message(current->channelId, "Please enter a number above 0.");
        return;
    }

    number--; // start at 0
    auto it = hands.begin();
    while(number) {
        it++;
        number--;
    }

    aegis::snowflake user = *it;
    std::string username = getUsername(user, current->guildId, _aegisCore);
    
    _bot->_handRepo->lower(current->guildId, user);

    _aegisCore->create_message(current->channelId, "```" + username + "```");
}

CommandInfo HandsCommand::getCommandInfo() {
    return {
        "hand",
        {"h"},
        "Show of hands. Raise or lower your hand to indicate a question or an answer to one.",
        {
            "up: raise your hand",
            "down: lower your hand",
            "list: list all users with their hand raised",
            "next: (admin only) show the next user with a raised hand, and lower it",
            "pick [number]: (admin only) pick a user from the list",
            "random: (admin only) pick a random user with their hand raised, and lower it",
            "clear: (admin only) lower all hands"
        }
    };
}