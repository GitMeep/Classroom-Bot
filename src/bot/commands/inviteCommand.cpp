#include <cbpch.h>

#include <bot/commands/inviteCommand.h>

void InviteCommand::call(const std::vector<std::string>& parameters, CommandContext* ctx) {
    m_AegisCore->create_dm_message(ctx->getUserId(), "Invite me to your server using this link: \nhttps://discordapp.com/api/oauth2/authorize?client_id=691945666896855072&permissions=297888850&scope=bot");
}

CommandInfo InviteCommand::getCommandInfo() {
    return {
        "invite",
        {},
        "Like the bot? Want it on your server? Use this command to get an invite link in your DM's.",
        {}
    };
}