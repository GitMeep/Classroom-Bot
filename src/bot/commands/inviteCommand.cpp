#include "inviteCommand.h"

#include <dpp/fmt/format.h>

#include <bot/localization/localization.h>
#include <bot/bot.h>

InviteCommand::InviteCommand() {
    {
        auto& command = m_Spec.commands.emplace_back(Localization::getString("invite_cmd_invite"), Localization::getString("invite_cmd_invite_desc"), NULL);

        for(const auto& lang : Localization::getLanguages()) {
            const std::string& currentLangCode = lang.first;

            if(currentLangCode == "en-US") continue;

            addLocalizationIfExists(command, currentLangCode, "invite_cmd_invite", "invite_cmd_invite_desc");
        }
    }
}

void InviteCommand::command(const CommandContext& ctx) {
    using namespace dpp;
    static std::string permissions = std::to_string(p_mute_members | p_send_messages | p_send_messages_in_threads |
                                                    p_embed_links | p_attach_files | p_use_external_emojis |
                                                    p_use_external_stickers | p_add_reactions);

    ctx.replyUnlocalized(
        dpp::message().add_component(
            dpp::component().add_component(
                dpp::component().set_type(dpp::cot_button).set_style(dpp::cos_link).set_label(ctx.localize("invite_label"))
                .set_url("https://discord.com/api/oauth2/authorize?client_id=" + std::to_string(ClassroomBot::cluster().me.id) + "&permissions=" + permissions + "&scope=bot%20applications.commands")
            )
        ).set_flags(dpp::m_ephemeral)
    );
}
