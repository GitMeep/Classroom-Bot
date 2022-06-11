#pragma once

#include <bot/commands/command.h>

/* hand-related commands
 *
 * /raise               (raise hand)
 * 
 * /lower               (lower hand)
 * 
 * /hands               (list hands)
 * 
 * /pick next           (pick next user in queue)
 * /pick user @user     (pick specific user)
 * /pick list           (show pick menu)
 * /pick random         (pick random user)
 * /pick clear          (clear hands)
*/

class HandsCommand : public Command {
public:
    HandsCommand();

    void command(const CommandContext& ctx);
    void userContext(const CommandContext& ctx);
    void selectClick(const CommandContext& ctx);

private:
    // slash commands
    void raiseCmd(const CommandContext& ctx);

    void lowerCmd(const CommandContext& ctx);

    void handsCmd(const CommandContext& ctx);

    void pickCmd(const CommandContext& ctx);
        // /pick subcommands
        void pickNext(const CommandContext& ctx);
        void pickUser(const CommandContext& ctx);
        void pickList(const CommandContext& ctx);
        void pickRandom(const CommandContext& ctx);
        void pickClear(const CommandContext& ctx);

    // select menu
    void pickFromList(const CommandContext& ctx);

    // user context
    void pickUserContext(const CommandContext& ctx);
};