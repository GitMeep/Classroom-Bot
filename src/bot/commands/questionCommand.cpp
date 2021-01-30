#include <bot/commands/questionCommand.h>
#include <bot/utils/utils.h>
#include <bot/bot.h>
#include <bot/persistence/repo/questionRepo.h>
#include <bot/persistence/model/question.h>

void QuestionCommand::call(const std::vector<std::string>& parameters, CommandContext* ctx) {
    if(ctx->isDM()) {
        ctx->respond("no_dm");
        return;
    }

    if(parameters.size() == 0) {
        ctx->respond("question_usage");
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
        ask(ctx, ss.str());
    }

    else if(verb == "list") {
        list(ctx);
    }

    else if(verb == "next") {
        if(!ctx->isAdmin()) {
            ctx->respond("admin_required");
        } else {
            next(ctx);
        }
    }

    else if(verb == "clear") {
        if(!ctx->isAdmin()) {
            ctx->respond("admin_required");
        } else {
            clear(ctx);
        }
    }

    else {
        ctx->respond("question_usage");
    }
}

void QuestionCommand::ask(CommandContext* ctx, std::string question) {
    auto questions = m_Bot->getQuestionRepo()->get(ctx->getChannelId());
    
    if(questions.size() >= 50) {
        ctx->respond("question_limit");
        return;
    }
    
    m_Bot->getQuestionRepo()->ask(ctx->getChannelId(), ctx->getUserId(), question);
    ctx->confirm();
}

void QuestionCommand::list(CommandContext* ctx) {
    auto questions = m_Bot->getQuestionRepo()->get(ctx->getChannelId());

    if(!questions.size()) {
        ctx->respond("no_questions");
        return;
    }

    std::stringstream ss;
    ss << "```" << std::endl;

    auto it = questions.cbegin();
    while (it != questions.cend()) {
        ss << getUsername(it->userId, ctx->getGuildId()) << ": " << it->question << std::endl;
        it++;
    }

    ss << "```" << std::endl;

    ctx->respondUnlocalized(ss.str());
}

void QuestionCommand::next(CommandContext* ctx) {
    auto questions = m_Bot->getQuestionRepo()->get(ctx->getChannelId());

    if(questions.size() == 0) {
        ctx->respond("no_questions");
        return;
    }

    Question question = questions.front();
    
    m_Bot->getQuestionRepo()->dismiss(ctx->getChannelId(), question.userId);

    std::string username = getUsername(question.userId, ctx->getGuildId());
    ctx->respondUnlocalized(fmt::format("```{0}: {1}```", username, question.question));
}

void QuestionCommand::clear(CommandContext* ctx) {
    m_Bot->getQuestionRepo()->clear(ctx->getChannelId());
    ctx->confirm();
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

