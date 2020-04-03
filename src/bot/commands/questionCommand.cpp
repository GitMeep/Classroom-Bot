#include "questionCommand.h"
#include "../utils/utils.h"

#define ADMIN_ROLE "Teacher"

const std::string actionMsg = "Please enter a valid action! Options are: ```ask, list, next, clear```";

QuestionCommand::QuestionCommand() {
    _log = spdlog::stdout_color_mt("QuestionCommand");
}

void QuestionCommand::call(std::vector<std::string> parameters, aegis::gateway::events::message_create& message) {
    _questionsMtx.lock();
    if(parameters.size() == 0) {
        message.channel.create_message(actionMsg);
    }
    std::string verb = parameters[0];
    if(verb == "ask") {
        std::string question;
        std::stringstream ss;
        auto i = parameters.begin();
        i++;
        while(i != parameters.end()) {
            ss << *i++ << " ";
        }
        std::string response;
        response = ask(ss.str(), message.channel.get_guild_id(), message.get_user());
        message.channel.create_message(response);
        message.msg.create_reaction("%E2%9C%85"); // checkmark emoji
    }

    else if(verb == "list") {
        std::string response = list(message.channel.get_guild_id());
        message.channel.create_message(response);
    }

    else if(verb == "next") {
        if(!isTeacher(message)) {
            message.channel.create_message(fmt::format("You must have the role \"{0}\" to use this command!", ADMIN_ROLE));
            return;
        } else {
            std::string response;
            response = next(message.channel.get_guild_id());
            message.channel.create_message(response);
        }
    }

    else if(verb == "clear") {
        if(!isTeacher(message)) {
            message.channel.create_message(fmt::format("You must have the role \"{0}\" to use this command!", ADMIN_ROLE));
            return;
        } else {
            std::string response;
            response = clear(message.channel.get_guild_id());
            message.channel.create_message(response);
        }
    }

    else {
        message.channel.create_message(actionMsg);
    }
    
    _questionsMtx.unlock();
}

std::string QuestionCommand::ask(std::string question, aegis::snowflake guildId, std::reference_wrapper<aegis::user> user) {
    if(!_questions.count(guildId)) {
        _questions[guildId]; // if a space in the map is not allocated for this guild, allocate one
    }
    
    if(_questions[guildId].size() > 100) {
        return "Queue limit reached! (100 questions)";
    }
    
    _questions[guildId].push_back(Question({user, question}));
}

std::string QuestionCommand::list(aegis::snowflake guildId) {
    auto it = _questions[guildId].begin();


    if(!_questions.count(guildId)) {
        return "No questions";
    }

    std::stringstream ss;
    ss << "```" << std::endl;

    for (auto i = _questions[guildId].cbegin(); i != _questions[guildId].cend(); ++i) {
        ss << getUsername(i->user, guildId) << ": " << i->question << std::endl;
    }

    ss << "```" << std::endl;

    return ss.str();
}

std::string QuestionCommand::next(aegis::snowflake guildId) {
    if(_questions[guildId].size() == 0) {
        return "No questions";
    }

    Question question = _questions[guildId].front();
    _questions[guildId].pop_front();

    if(_questions[guildId].size() == 0) {
        _questions.erase(guildId);
    }

    std::string username = getUsername(question.user.get(), guildId);
    return fmt::format("```{0}: {1}```", username, question.question);
}

std::string QuestionCommand::clear(aegis::snowflake guildId) {
    _questions[guildId].clear();
    _questions.erase(guildId);
    return "Questions cleared!";
}

CommandInfo QuestionCommand::getCommandInfo() {
    return {
        {"question", "q"},
        "Ask questions",
        {
            "ask [question]: ask a question",
            "list: list all questions",
            "next: (teacher only) show the next question, and remove it from queue",
            "clear: (teacher only) clear the question queue"
        }
    };
}

