#include <bot/commands/questionCommand.h>
#include <bot/utils/utils.h>
#include <bot/bot.h>
#include <bot/persistence/repo/questionRepo.h>
#include <bot/persistence/model/question.h>

const std::string actionMsg = "Please enter a valid action! Options are: ```ask, list, next, clear```";

void QuestionCommand::call(const std::vector<std::string>& parameters, MessageInfo* current) {
    if(current->isDm) {
        m_AegisCore->create_dm_message(current->userId, "Command not supported in DM's");
        return;
    }

    if(parameters.size() == 0) {
        m_AegisCore->create_message(current->channelId, actionMsg);
        return;
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
        if(!isTeacher(current->guildId, current->userId)) {
            m_AegisCore->create_message(current->channelId, "You must have the admin role to use this command.");
        } else {
            next(current);
        }
    }

    else if(verb == "clear") {
        if(!isTeacher(current->guildId, current->userId)) {
            m_AegisCore->create_message(current->channelId, "You must have the admin role to use this command.");
        } else {
            clear(current);
        }
    }

    else {
        m_AegisCore->create_message(current->channelId, actionMsg);
    }
}

void QuestionCommand::ask(MessageInfo* current, std::string question) {
    auto questions = m_Bot->getQuestionRepo()->get(current->guildId);
    
    if(questions.size() >= 50) {
        m_AegisCore->create_message(current->channelId, "Queue limit reached (50 questions)");
        return;
    }
    
    m_Bot->getQuestionRepo()->ask(current->channelId, current->userId, question);

    m_AegisCore->find_channel(current->channelId)->create_reaction(aegis::create_reaction_t().message_id(current->messageId).emoji_text("%E2%9C%85"));
}

void QuestionCommand::list(MessageInfo* current) {
    auto questions = m_Bot->getQuestionRepo()->get(current->channelId);

    if(!questions.size()) {
        m_AegisCore->create_message(current->channelId, "No questions");
        return;
    }

    std::stringstream ss;
    ss << "```" << std::endl;

    auto it = questions.cbegin();
    while (it != questions.cend()) {
        ss << getUsername(it->userId, current->guildId) << ": " << it->question << std::endl;
        it++;
    }

    ss << "```" << std::endl;

    m_AegisCore->create_message(current->channelId, ss.str());
}

void QuestionCommand::next(MessageInfo* current) {
    auto questions = m_Bot->getQuestionRepo()->get(current->channelId);

    if(questions.size() == 0) {
        m_AegisCore->create_message(current->channelId, "No questions");
        return;
    }

    Question question = questions.front();
    
    m_Bot->getQuestionRepo()->dismiss(current->channelId, question.userId);

    std::string username = getUsername(question.userId, current->guildId);
    m_AegisCore->create_message(current->channelId, fmt::format("```{0}: {1}```", username, question.question));
}

void QuestionCommand::clear(MessageInfo* current) {
    m_Bot->getQuestionRepo()->clear(current->channelId);
    m_AegisCore->find_channel(current->channelId)->create_reaction(aegis::create_reaction_t().message_id(current->messageId).emoji_text("%E2%9C%85"));
}


CommandInfo QuestionCommand::getCommandInfo() {
    return {
        "question",
        {"q"},
        "Ask questions that a teacher can then answer one by one.",
        {
            "ask [question]: ask a question",
            "list: list all questions",
            "next: (admin only) show the next question, and remove it from the queue",
            "clear: (admin only) clear the question queue"
        }
    };
}

