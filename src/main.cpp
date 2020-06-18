#include "cbpch.h"

#include "bot/bot.h"

#include "bot/commands/questionCommand.h"
#include "bot/commands/handsCommand.h"
#include "bot/commands/inviteCommand.h"
#include "bot/commands/settingsCommad.h"
#include "bot/commands/pchemCommand.h"
#include "bot/commands/helpCommand.h"
#include "bot/commands/muteCommand.h"

#include "bot/config/config.h"

int main(int argc, char *argv[])
{
    auto log = spdlog::stdout_color_mt("classroombot");
    log->set_pattern("%^%Y-%m-%d %H:%M:%S.%e [%L] [th#%t]%$ : %v");
    log->set_level(spdlog::level::trace);

    std::shared_ptr<Config> config = std::make_shared<Config>();
    try {
        config->loadFromFile("config.json");
    } catch (std::runtime_error& e) {
        log->error(std::string(e.what()));
        return 1;
    }

    std::string token = config->getValue("bot")["token"];

    try
    {
        ClassroomBot& classroomBot = ClassroomBot::get();
        classroomBot.init(token, config);

        classroomBot.registerCommand(new QuestionCommand());
        classroomBot.registerCommand(new HandsCommand());
        classroomBot.registerCommand(new InviteCommand());
        classroomBot.registerCommand(new SettingsCommand());
        classroomBot.registerCommand(new PchemCommand());
        classroomBot.registerCommand(new HelpCommand());
        classroomBot.registerCommand(new MuteCommand());

        classroomBot.run();

        return 0;
    }
    catch (std::runtime_error &e)
    {
        log->error("Bot failed with error message: \n" + std::string(e.what()));
        return 1;
    }
}