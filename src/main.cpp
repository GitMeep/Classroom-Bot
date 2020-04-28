#include <aegis.hpp>
#include <spdlog/spdlog.h>

#include "bot/bot.h"

#include "bot/commands/questionCommand.h"
#include "bot/commands/handsCommand.h"
#include "bot/commands/muteCommand.h"
#include "bot/commands/inviteCommand.h"
#include "bot/commands/settingsCommad.h"

using json = nlohmann::json;

int main(int argc, char *argv[])
{
    auto log = spdlog::stdout_color_mt("aegis");
    log->set_pattern("%^%Y-%m-%d %H:%M:%S.%e [%L] [th#%t]%$ : %v");
    log->set_level(spdlog::level::trace);

    log->info("Starting ClassroomBot version " + std::string(BOT_VERSION));

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
        std::shared_ptr<ClassroomBot> classroomBot = std::make_shared<ClassroomBot>(token, log, config);

        classroomBot->registerCommand(new QuestionCommand(classroomBot));
        classroomBot->registerCommand(new HandsCommand(classroomBot));
        classroomBot->registerCommand(new MuteCommand(classroomBot));
        classroomBot->registerCommand(new InviteCommand(classroomBot));
        classroomBot->registerCommand(new SettingsCommand(classroomBot));

        classroomBot->run();
        return 0;
    }
    catch (std::runtime_error &e)
    {
        log->error("Bot failed with error message: \n" + std::string(e.what()));
        return 1;
    }
}