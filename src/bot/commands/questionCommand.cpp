#include <bot/commands/questionCommand.h>
#include <bot/utils/utils.h>
#include <bot/bot.h>
#include <bot/persistence/repo/questionRepo.h>
#include <bot/persistence/model/question.h>

void QuestionCommand::call(int verb, const std::vector<std::string>& parameters, CommandContext* ctx) {
   switch(verb) {
    case 1: // ask
        ask(ctx, parameters);
        break;
    case 2: // list
        list(ctx);
        break;
    case 3: // next
        next(ctx);
        break;
    case 4: // clear
        clear(ctx);
        break;
    default:
    case 0:
        ctx->respond("question_usage");
        break;
   }

}

void QuestionCommand::ask(CommandContext* ctx, const std::vector<std::string>& parameters) {
    std::string question = "";
    auto i = parameters.begin();
    i++;
    while(i != parameters.end()) {
        question += *i++;
        if(i != parameters.end()) question += " ";
    }

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
    if(!ctx->isAdmin()) {
        ctx->respond("admin_required");
        return;
    }

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
    if(!ctx->isAdmin()) {
        ctx->respond("admin_required");
        return;
    }

    m_Bot->getQuestionRepo()->clear(ctx->getChannelId());
    ctx->confirm();
}


CommandInfo QuestionCommand::getCommandInfo() {
    return {
        "question",
        "question_cmd",
        {"question_alias"},
        "question_desc",
        {
            "question_option_ask_desc",
            "question_option_list_desc",
            "question_option_next_desc",
            "question_option_clear_desc"
        },
        {
            "question_option_ask",
            "question_option_list",
            "question_option_next",
            "question_option_clear"
        },
        true
    };
}

