#include <aegis.hpp>
#include <spdlog/spdlog.h>
#include <signal.h>
#include <fstream>
#include <execinfo.h>
#include <ctime>

#include "bot/bot.h"

#include "bot/commands/questionCommand.h"
#include "bot/commands/handsCommand.h"
#include "bot/commands/muteCommand.h"
#include "bot/commands/inviteCommand.h"
#include "bot/commands/settingsCommad.h"
#include "bot/commands/pchemCommand.h"

using json = nlohmann::json;

#define BT_BUF_SIZE 100

void segvHandler(int sig) {
    std::cout << "Segmentation fault encountered. Please report this." << std::endl;
    
    // code shamelessly stolen from backtrace manpage
    int j, nptrs;
    void *buffer[BT_BUF_SIZE];
    char **strings;

    nptrs = backtrace(buffer, BT_BUF_SIZE);

    /* The call backtrace_symbols_fd(buffer, nptrs, STDOUT_FILENO)
        would produce similar output to the following: */

    strings = backtrace_symbols(buffer, nptrs);
    if (strings == NULL) {
        perror("backtrace_symbols");
        exit(EXIT_FAILURE);
    }

    time_t now = time(0);
    char* dt = ctime(&now);

    std::ofstream ofs;
    ofs.open ("crashes.txt", std::ofstream::out | std::ofstream::app);
    ofs << "Crash on " << dt;
    for (j = 0; j < nptrs; j++) {
        ofs << strings[j] << std::endl;
        printf("%s\n", strings[j]);
    }
    ofs << std::endl;
    ofs.close();
    
    free(strings);

    exit(1);
    
}

int main(int argc, char *argv[])
{
    signal(SIGSEGV, segvHandler);
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
        classroomBot->registerCommand(new PchemCommand(classroomBot));

        classroomBot->run();
        return 0;
    }
    catch (std::runtime_error &e)
    {
        log->error("Bot failed with error message: \n" + std::string(e.what()));
        return 1;
    }
}