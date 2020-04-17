#include <aegis.hpp>

#include "bot/bot.h"

#include "bot/commands/questionCommand.h"
#include "bot/commands/handsCommand.h"
#include "bot/commands/muteCommand.h"
#include "bot/commands/inviteCommand.h"

using json = nlohmann::json;

int main(int argc, char * argv[])
{
    if(argc < 2) return 1;
    aegis::core bot(aegis::create_bot_t().log_level(spdlog::level::trace).token(argv[1]));
    
    ClassroomBot classroomBot(&bot);

    classroomBot.registerCommand(new QuestionCommand(&classroomBot));
    classroomBot.registerCommand(new HandsCommand(&classroomBot));
    classroomBot.registerCommand(new MuteCommand(&classroomBot));
    classroomBot.registerCommand(new InviteCommand(&classroomBot));

    AEGIS_TRACE(bot.log, "Bot object created");
    // With min log level set to trace and wsdbg (websocket debug) set to true
    // the bot will dump all websocket messages to console
    bot.wsdbg = true;
    // start the bot
    bot.run();
    // yield thread execution to the library
    bot.yield();
    std::cout << "Press any key to continue...\n";
    std::cin.ignore();
    return 0;
}