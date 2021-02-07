#include <cbpch.h>

#include <bot/commands/inviteCommand.h>
#include <bot/bot.h>
#include <bot/localization/localization.h>

void InviteCommand::call(int verb, const std::vector<std::string>& parameters, CommandContext* ctx) {
    std::string response = m_Bot->getLocalization()->getString(ctx->getSettings().lang, "invite_response");
    m_AegisCore->create_dm_message(ctx->getUserId(), response);
}

CommandInfo InviteCommand::getCommandInfo() {
    return {
        "invite",
        "invite_cmd",
        {},
        "invite_desc",
        {}
    };
}