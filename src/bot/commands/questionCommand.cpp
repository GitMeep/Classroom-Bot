#include <bot/commands/questionCommand.h>
#include <bot/utils/utils.h>
#include <spdlog/spdlog.h>

void QuestionCommand::call(std::vector<std::string> parameters, aegis::gateway::events::message_create& message) {
    std::string verb = parameters[0];
    if(verb == "ask") {
        std::string question;
        std::stringstream ss;
        auto i = parameters.begin();
        i++;
        while(i != parameters.end()) {
            ss << *i++ << " ";
        }
        message.channel.create_reaction(message.msg.get_id(),"white_check_mark:212e30e47232be03033a87dc58edaa95");
        ask(ss.str(), message.channel.get_guild_id(), message.get_user());
        // message.msg.get_channel().create_message("Registered");
        return;
    }

    if(verb == "list") {
        std::string response = list(message.channel.get_guild_id());
    }
}

void QuestionCommand::ask(std::string question, aegis::snowflake guildId, std::reference_wrapper<aegis::user> user) {
    _questions[guildId].push_back(Question({user, question}));
}

std::string QuestionCommand::list(aegis::snowflake guildId) {
    auto it = _questions[guildId].begin();

    if(_questions[guildId].size() == 0) {
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

/*
void questionsbot::next(std::vector<std::string> parameters, aegis::gateway::events::message_create message) {
    if(!isTeacher(message)) {
        message.channel.create_message(fmt::format("You must have the role \"{0}\" to use this command!", ADMIN_ROLE));
        return;
    }

    if(_questions[message.channel.get_guild_id()].size() == 0) {
        message.channel.create_message("No questions");
        return;
    }

    question question = _questions[message.channel.get_guild_id()].front();
    _questions[message.channel.get_guild_id()].pop_front();

    std::string username = getUsername(question.user.get(), message.channel.get_guild_id());
    message.channel.create_message(fmt::format("```{0}: {1}```", username, question.question));
}

void questionsbot::clear(std::vector<std::string> parameters, aegis::gateway::events::message_create message) {
    if(!isTeacher(message)) {
        message.channel.create_message(fmt::format("You must have the role \"{0}\" to use this command!", ADMIN_ROLE));
        return;
    }

    _questions[message.channel.get_guild_id()].clear();
    message.channel.create_message("Questions cleared!");
}
*/


CommandInfo QuestionCommand::getCommandInfo() {
    return {
        {"question", "q"}
    };
}

