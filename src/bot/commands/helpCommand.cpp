#include <bot/commands/helpCommand.h>

void HelpCommand::call(std::vector<std::string> parameters, aegis::gateway::events::message_create& message) {
    std::stringstream ss;
    ss << "Commands ```" << std::endl;
    ss << "?help: Show this help" << std::endl;
    ss << "Aliases:" << std::endl;
    ss << "\t?h" << std::endl;
    ss << "------------------------------------------------" << std::endl << std::endl;

    ss << "?question [action]" << std::endl;
    ss << "Aliases:" << std::endl;
    ss << "\t?q" << std::endl;
    ss << "Actions:" << std::endl;
    ss << "\task [question]: ask a question" << std::endl;
    ss << "\tlist: list all questions" << std::endl;
    ss << "\tnext: (teacher only) show the next question, and remove it from queue" << std::endl;
    ss << "\tclear: (teacher only) clear the question queue" << std::endl;
    ss << "------------------------------------------------" << std::endl << std::endl;

    ss << "?hand [action]" << std::endl;
    ss << "Aliases:" << std::endl;
    ss << "\t?ha" << std::endl;
    ss << "Actions:" << std::endl;
    ss << "\tup: raise your hand" << std::endl;
    ss << "\tdown: lower your hand" << std::endl;
    ss << "\tlist: list all users with their hand raised" << std::endl;
    ss << "\tnext: (teacher only) show the next user with a raised hand, and lower it" << std::endl;
    ss << "\tpick: [number] (teacher only) pick a user from the list" << std::endl;
    ss << "\trandom: (teacher only) pick a random user with their hand raised, and lower it" << std::endl;
    ss << "\tclear: (tacher only) lower all hands" << std::endl;

    ss << "```";
    message.channel.create_message(ss.str());
}


CommandInfo HelpCommand::getCommandInfo() {
    return {
        {"help", "h"}
    };
}