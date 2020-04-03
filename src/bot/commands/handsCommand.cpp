#include <bot/commands/handsCommand.h>
#include <bot/utils/utils.h>

#include <stdlib.h>
#include <time.h>

#define ADMIN_ROLE "Teacher"

const std::string actionMsg = "Please enter a valid action! Options are: ```up, down, next, pick, random, list, clear```";

void HandsCommand::call(std::vector<std::string> parameters, aegis::gateway::events::message_create& message) {
    _handsMtx.lock();
    if(parameters.size() == 0) {
        message.channel.create_message(actionMsg);
        return;
    }

    std::string verb = parameters[0];
    if(verb == "up") {
        std::string response = "";
        response = up(message.channel.get_guild_id(), message.user.value().get());
        if(response != "") {
            message.channel.create_message(response);
        } else {
            message.msg.create_reaction("%E2%9C%85"); // checkmark emoji
        }
    }

    else if(verb == "down") {
        down(message.channel.get_guild_id(), message.user.value().get());
        message.msg.create_reaction("%E2%9C%85"); // checkmark emoji
    }

    else if(verb == "list") {
        std::string response;
        response = list(message.channel.get_guild_id());
        message.channel.create_message(response);
    }

    else if(verb == "next") {
        if(!isTeacher(message)) {
            message.channel.create_message(fmt::format("You must have the role \"{0}\" to use this command!", ADMIN_ROLE));
            _handsMtx.unlock();
            return;
        } else {
            std::string response;
            response = next(message.channel.get_guild_id());
            message.channel.create_message(response);
        }
    }

    else if(verb == "random") {
        if(!isTeacher(message)) {
            message.channel.create_message(fmt::format("You must have the role \"{0}\" to use this command!", ADMIN_ROLE));
            _handsMtx.unlock();
            return;
        } else {
            std::string response;
            response = random(message.channel.get_guild_id());
            message.channel.create_message(response);
        }
    }

    else if(verb == "clear") {
        if(!isTeacher(message)) {
            message.channel.create_message(fmt::format("You must have the role \"{0}\" to use this command!", ADMIN_ROLE));
            _handsMtx.unlock();
            return;
        } else {
            clear(message.channel.get_guild_id());
            message.msg.create_reaction("%E2%9C%85"); // checkmark emoji
        }
    } else if(verb == "pick") {
        if(!isTeacher(message)) {
            message.channel.create_message(fmt::format("You must have the role \"{0}\" to use this command!", ADMIN_ROLE));
            _handsMtx.unlock();
            return;
        } else {
            int number;
            try {
                number = std::stoi(parameters[1]);
            } catch(std::invalid_argument) {
                message.channel.create_message("Please enter a valid number");
                return;
            } catch(std::out_of_range) {
                message.channel.create_message("Please enter a reasonable number");
                return;
            }
            std::string response;
            response = pick(message.channel.get_guild_id(), number);
            message.channel.create_message(response);
        }
    }

    else {
        message.channel.create_message(actionMsg);
    }
    
    _handsMtx.unlock();
}

std::string HandsCommand::up(aegis::snowflake guildId, aegis::user& user) {
    for(auto handUser : _hands[guildId]) {
        if(handUser == &user) {
            return "You already have your hand raised!";
        }
    }

    _hands[guildId].emplace_back(&user);
    return "";
}

std::string HandsCommand::down(aegis::snowflake guildId, aegis::user& user) {
    _hands[guildId].remove(&user);

    if(_hands[guildId].size() == 0) {
        _hands.erase(guildId);
    }

    return "";
}

std::string HandsCommand::next(aegis::snowflake guildId) {
    if(_hands[guildId].size() == 0) {
        return "No hands up";
    }
    aegis::user* user = _hands[guildId].front();
    std::string username = getUsername(*user, guildId);
    _hands[guildId].pop_front();
    
    if(_hands[guildId].size() == 0) {
        _hands.erase(guildId);
    }

    return "```" + username + "```";
}

std::string HandsCommand::clear(aegis::snowflake guildId) {
    _hands[guildId].clear();
    _hands.erase(guildId);
    return "Done";
}

std::string HandsCommand::list(aegis::snowflake guildId) {
    if(!_hands.count(guildId)) {
        return "No hands up";
    }
    std::stringstream ss;
    ss << "Users with their hand up:```";

    int number = 1;
    auto it = _hands[guildId].begin();
    while(it != _hands[guildId].end()) {
        ss << number << ": " << getUsername(**it, guildId) << "\n";
        it++;
        number++;
    }
    ss << "```\n";
    return ss.str();
}

std::string HandsCommand::random(aegis::snowflake guildId) {
    if(_hands[guildId].size() == 0) {
        return "No hands up";
    }
    srand(time(NULL));
    int random = rand() % _hands[guildId].size(); // random number between 0 and size of guild hands

    auto it = _hands[guildId].begin();
    while(random) { // go to random element in guild hands
        it++;
        random--;
    }

    aegis::user* user = *it;
    std::string username = getUsername(*user, guildId);
    _hands[guildId].remove(user);

    if(_hands[guildId].size() == 0) {
        _hands.erase(guildId);
    }

    return "```" + username + "```";
}

std::string HandsCommand::pick(aegis::snowflake guildId, int number) {
    if(!_hands.count(guildId)) {
        return "No one has their hand up!";
    }

    if(number > _hands[guildId].size()) {
        return "There aren't that many users with their hands up";
    }

    if(number < 1) {
        return "Please enter a number above 0!";
    }

    number--; // start at 0
    auto it = _hands[guildId].begin();
    while(number) {
        it++;
        number--;
    }

    aegis::user* user = *it;
    std::string username = getUsername(*user, guildId);
    _hands[guildId].remove(user);

    if(_hands[guildId].size() == 0) {
        _hands.erase(guildId);
    }

    return "```" + username + "```";
}

CommandInfo HandsCommand::getCommandInfo() {
    return {
        {"hand", "ha"}
    };
}