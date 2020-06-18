#include "questionCommand.h"
#include "bot/utils/utils.h"
#include "bot/bot.h"
#include "bot/persistence/repo/questionRepo.h"
#include "bot/persistence/model/question.h"

const std::string actionMsg = "Please enter a valid action! Options are: ```ask, list, next, clear```";

void QuestionCommand::call(const std::vector<std::string>& parameters, MessageInfo* current) {
    if(current->isDm) {
        _aegisCore->create_dm_message(current->userId, "Command not supported in DM's");
        return;
    }

    if(parameters.size() == 0) {
        _aegisCore->create_message(current->channelId, actionMsg);
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
        ask(current, ss.str());
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

    else if(verb == "clear") {
        if(!isTeacher(current->guildId, current->userId, _aegisCore, _bot->_settingsRepo)) {
            _aegisCore->create_message(current->channelId, "You must have the admin role to use this command.");
        } else {
            clear(current);
        }
    }

    else {
        _aegisCore->create_message(current->channelId, actionMsg);
    }
}

void QuestionCommand::ask(MessageInfo* current, std::string question) {
    auto questions = _bot->_questionRepo->get(current->guildId);
    
    if(questions.size() > 100) {
        _aegisCore->create_message(current->channelId, "Queue limit reached! (100 questions)");
        return;
    }
    
    _bot->_questionRepo->ask(current->guildId, current->userId, question);

    _aegisCore->find_channel(current->channelId)->create_reaction(aegis::create_reaction_t().message_id(current->messageId).emoji_text("%E2%9C%85"));
}

void QuestionCommand::list(MessageInfo* current) {
    auto questions = _bot->_questionRepo->get(current->guildId);

    if(!questions.size()) {
        _aegisCore->create_message(current->channelId, "No questions");
        return;
    }

    std::stringstream ss;
    ss << "```" << std::endl;

    for (auto i = questions.cbegin(); i != questions.cend(); ++i) {
        ss << getUsername(i->userId, current->guildId, _aegisCore) << ": " << i->question << std::endl;
    }

    ss << "```" << std::endl;

    _aegisCore->create_message(current->channelId, ss.str());
}

void QuestionCommand::next(MessageInfo* current) {
    auto questions = _bot->_questionRepo->get(current->guildId);

    if(questions.size() == 0) {
        _aegisCore->create_message(current->channelId, "No questions");
        return;
    }

    Question question = questions.front();
    
    _bot->_questionRepo->dismiss(current->guildId, question.userId);

    std::string username = getUsername(question.userId, current->guildId, _aegisCore);
    _aegisCore->create_message(current->channelId, fmt::format("```{0}: {1}```", username, question.question));
}

void QuestionCommand::clear(MessageInfo* current) {
    _bot->_questionRepo->clear(current->guildId);
    _aegisCore->find_channel(current->channelId)->create_reaction(aegis::create_reaction_t().message_id(current->messageId).emoji_text("%E2%9C%85"));
}

CommandInfo QuestionCommand::getCommandInfo() {
    return {
        "question",
        {"q"},
        "Ask questions that a teacher can then answer one by one.",
        {
            "ask [question]: ask a question",
            "list: list all questions",
            "next: (admin only) show the next question, and remove it from queue",
            "clear: (admin only) clear the question queue"
        }
    };
}

