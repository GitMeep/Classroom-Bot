#include <bot/bot.h>
#include <bot/config/config.h>

#include <bot/commands/questionCommand.h>
#include <bot/commands/handsCommand.h>
#include <bot/commands/inviteCommand.h>
#include <bot/commands/settingsCommand.h>
#include <bot/commands/helpCommand.h>
#include <bot/commands/muteCommand.h>

#include <execinfo.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <iostream>

void print_backtrace(int sig) {
    printf("Signal: %d", sig);
    int j, nptrs;
    void *buffer[100];
    char **strings;

    nptrs = backtrace(buffer, 100);

    strings = backtrace_symbols(buffer, nptrs);
    if (strings == NULL) {
        perror("backtrace_symbols");
        exit(EXIT_FAILURE);
    }

   for (j = 0; j < nptrs; j++)
        printf("%s\n", strings[j]);

   free(strings);

   exit(1);
}

int main() {
    signal(SIGSEGV, print_backtrace);
    signal(SIGBUS, print_backtrace);

    try
    {
        ClassroomBot& classroomBot = ClassroomBot::getBot();
        classroomBot.init();

        classroomBot.registerCommand(new QuestionCommand());
        classroomBot.registerCommand(new HandsCommand());
        classroomBot.registerCommand(new InviteCommand());
        classroomBot.registerCommand(new SettingsCommand());
        classroomBot.registerCommand(new HelpCommand());
        classroomBot.registerCommand(new MuteCommand());

        classroomBot.run();

        return 0;
    }
    catch (std::system_error &e)
    {
        std::cout << "Bot failed with error message: \n" << std::string(e.what()) << std::endl;
        print_backtrace(0);
        return 1;
    }
}