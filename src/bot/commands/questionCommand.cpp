#include "questionCommand.h"
#include "../utils/utils.h"

#define ADMIN_ROLE "Teacher"

const std::string actionMsg = "Please enter a valid action! Options are: ```ask, list, next, clear```";

void QuestionCommand::call(std::vector<std::string> parameters, CurrentCommand current) {
    std::lock_guard<std::mutex> guard(_questionsMtx);

    Command::call(parameters, current);

    if(parameters.size() == 0) {
        _aegisCore->create_message(_current.channelId, actionMsg);
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
        ask(ss.str());
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

    else if(verb == "clear") {
        if(!isTeacher(_current.guildId, _current.userId, _aegisCore, _bot->_settingsRepo)) {
            _aegisCore->create_message(_current.channelId, "You must have the admin role to use this command.");
        } else {
            clear();
        }
    }

    else {
        _aegisCore->create_message(_current.channelId, actionMsg);
    }
}

void QuestionCommand::ask(std::string question) {
    if(!_questions.count(_current.guildId)) {
        _questions[_current.guildId]; // if a space in the map is not allocated for this guild, allocate one
    }
    
    if(_questions[_current.guildId].size() > 100) {
        _aegisCore->create_message(_current.channelId, "Queue limit reached! (100 questions)");
        return;
    }
    
    _questions[_current.guildId].push_back(Question({_current.userId, question}));
    _aegisCore->find_channel(_current.channelId)->create_reaction(aegis::create_reaction_t().message_id(_current.messageId).emoji_text("%E2%9C%85"));
}

void QuestionCommand::list() {
    if(!_questions.count(_current.guildId)) {
        _aegisCore->create_message(_current.channelId, "No questions");
        return;
    }

    std::stringstream ss;
    ss << "```" << std::endl;

    for (auto i = _questions[_current.guildId].cbegin(); i != _questions[_current.guildId].cend(); ++i) {
        ss << getUsername(i->user, _current.guildId, _aegisCore) << ": " << i->question << std::endl;
    }

    ss << "```" << std::endl;

    _aegisCore->create_message(_current.channelId, ss.str());
}

void QuestionCommand::next() {
    if(_questions[_current.guildId].size() == 0) {
        _aegisCore->create_message(_current.channelId, "No questions");
        return;
    }

    Question question = _questions[_current.guildId].front();
    _questions[_current.guildId].pop_front();

    if(_questions[_current.guildId].size() == 0) {
        _questions.erase(_current.guildId);
    }

    std::string username = getUsername(question.user.get(), _current.guildId, _aegisCore);
    _aegisCore->create_message(_current.channelId, fmt::format("```{0}: {1}```", username, question.question));
}

void QuestionCommand::clear() {
    _questions[_current.guildId].clear();
    _questions.erase(_current.guildId);
    _aegisCore->find_channel(_current.channelId)->create_reaction(aegis::create_reaction_t().message_id(_current.messageId).emoji_text("%E2%9C%85"));
}

bool QuestionCommand::checkPermissions(aegis::permission channelPermissions) {
    return
    channelPermissions.can_add_reactions();
}

CommandInfo QuestionCommand::getCommandInfo() {
    return {
        {"question", "q"},
        "Ask questions",
        {
            "ask [question]: ask a question",
            "list: list all questions",
            "next: (admin only) show the next question, and remove it from queue",
            "clear: (admin only) clear the question queue"
        },
        "I need permission to add reactions to use this command!"
    };
}

