#include <bot/commands/helpCommand.h>

void HelpCommand::call(std::vector<std::string> parameters, aegis::gateway::events::message_create& message) {
    std::stringstream ss;
    ss << "```" << std::endl << "Commands" << std::endl;
    ss << "?help | h: Show this help page" << std::endl;
    ss << "?list: Shows a list of all questions" << std::endl;
    ss << "?clear: Removes all questions from the queue (can only be used by the \"Teacher\" role)" << std::endl;
    ss << "?next: Shows the next question, and removes it from the queue (can only be used by the \"Teacher\" role)" << std::endl;
    ss << "```";
    message.channel.create_message(ss.str());
}


CommandInfo HelpCommand::getCommandInfo() {
    return {
        {"help", "h"}
    };
}