#include <aegis.hpp>

#include "bot/bot.h"

#include "bot/commands/questionCommand.h"
#include "bot/commands/handsCommand.h"
#include "bot/commands/muteCommand.h"

using json = nlohmann::json;

int main(int argc, char * argv[])
{
    if(argc < 2) return 1;
    // Create bot object with a minimum log level of trace
    aegis::core bot(aegis::create_bot_t().log_level(spdlog::level::trace).token(argv[1]));
    
    ClassroomBot classroomBot(bot.log, &bot);

    classroomBot.registerCommand(new QuestionCommand(std::shared_ptr<aegis::core>(&bot)));
    classroomBot.registerCommand(new HandsCommand(std::shared_ptr<aegis::core>(&bot)));
    classroomBot.registerCommand(new MuteCommand(std::shared_ptr<aegis::core>(&bot)));

    AEGIS_TRACE(bot.log, "Bot object created");
    // With min log level set to trace and wsdbg (websocket debug) set to true
    // the bot will dump all websocket messages to console
    bot.wsdbg = true;
    // These callbacks are what are called when websocket events occur
    bot.set_on_message_create(std::bind(&ClassroomBot::onMessage, &classroomBot, std::placeholders::_1));
    // start the bot
    bot.run();
    // yield thread execution to the library
    bot.yield();
    std::cout << "Press any key to continue...\n";
    std::cin.ignore();
    return 0;
}