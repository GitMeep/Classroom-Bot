#include "inviteCommand.h"

void InviteCommand::call(std::vector<std::string> parameters, CurrentCommand current) {
    Command::call(parameters, current);

    _aegisCore->create_dm_message(current.userId, "Invite me to your server using this link: \nhttps://discordapp.com/api/oauth2/authorize?client_id=691945666896855072&permissions=297888850&scope=bot");
}

bool InviteCommand::checkPermissions(aegis::permission channelPermissions) {
    return true;
}

CommandInfo InviteCommand::getCommandInfo() {
    return {
        {"invite"},
        "Like the bot? Want it on your server? Use this command to get an invite link in your DM's. (Please)",
        {},
        "Something went very wrong"
    };
}