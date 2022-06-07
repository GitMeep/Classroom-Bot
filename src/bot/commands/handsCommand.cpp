#include <bot/commands/handsCommand.h>
#include <bot/localization/localization.h>
#include <bot/bot.h>
#include <bot/persistence/repo/handRepo.h>
#include <bot/util/util.h>

#include <dpp/fmt/format.h>

const std::string actionMsg = "Please enter a valid action. Options are: ```up, down, next, pick, random, list, clear```";
const std::string handPickMenuId = "hands_pick_user";

HandsCommand::HandsCommand() :
Command::Command() {
    
    /* hand-related commands
     *
     * /raise (raise hand)
     * 
     * /lower (lower hand)
     * 
     * /hand next (pick next user in queue)
     * /hand random (pick random user)
     * /hand list (list hands)
     * /hand clear (clear hands)
     * 
     * /pick (show pick menu)
     * /pick @user (pick specific user)
    */
    {
        // application ids are overwritten by main class when registering commands, set them to NULL for now
        dpp::slashcommand& raiseCommand   = m_Spec.commands.emplace_back(Localization::getString("hand_raise_cmd"),     Localization::getString("hand_raise_cmd_desc"), NULL);
        dpp::slashcommand& lowerCommand   = m_Spec.commands.emplace_back(Localization::getString("hand_lower_cmd"),     Localization::getString("hand_lower_cmd_desc"), NULL);
        dpp::slashcommand& handCommand    = m_Spec.commands.emplace_back(Localization::getString("hand_hand_cmd"),      Localization::getString("hand_hand_cmd_desc"),  NULL);
        dpp::slashcommand& pickCommand    = m_Spec.commands.emplace_back(Localization::getString("hand_pick_cmd"),      Localization::getString("hand_pick_cmd_desc"),  NULL);

        // options for /hand
        dpp::command_option optionHandNext  (dpp::co_sub_command,   Localization::getString("hand_hand_cmd_option_next"),   Localization::getString("hand_hand_cmd_option_next_desc"));
        dpp::command_option optionHandList  (dpp::co_sub_command,   Localization::getString("hand_hand_cmd_option_list"),   Localization::getString("hand_hand_cmd_option_list_desc"));
        dpp::command_option optionHandRandom(dpp::co_sub_command,   Localization::getString("hand_hand_cmd_option_random"), Localization::getString("hand_hand_cmd_option_random_desc"));
        dpp::command_option optionHandClear (dpp::co_sub_command,   Localization::getString("hand_hand_cmd_option_clear"),  Localization::getString("hand_hand_cmd_option_clear_desc"));

        // option for /pick
        dpp::command_option optionPickUser  (dpp::co_user,          Localization::getString("hand_pick_cmd_option_user"),   Localization::getString("hand_pick_cmd_option_desc"));

        for(const auto& lang : Localization::getLanguages()) {
            const std::string& currentLangCode = lang.first;

            if(currentLangCode == "en-US") continue;

            addLocalizationIfExists(raiseCommand,       currentLangCode, "hand_raise_cmd", "hand_raise_cmd_desc");
            addLocalizationIfExists(lowerCommand,       currentLangCode, "hand_lower_cmd", "hand_lower_cmd_desc");
            addLocalizationIfExists(handCommand,        currentLangCode, "hand_hand_cmd",  "hand_hand_cmd_desc");
            addLocalizationIfExists(pickCommand,        currentLangCode, "hand_pick_cmd",  "hand_pick_cmd_desc");

            addLocalizationIfExists(optionHandNext,     currentLangCode, "hand_hand_cmd_option_next",   "hand_hand_cmd_option_next_desc");
            addLocalizationIfExists(optionHandList,     currentLangCode, "hand_hand_cmd_option_list",   "hand_hand_cmd_option_list_desc");
            addLocalizationIfExists(optionHandRandom,   currentLangCode, "hand_hand_cmd_option_random", "hand_hand_cmd_option_random_desc");
            addLocalizationIfExists(optionHandClear,    currentLangCode, "hand_hand_cmd_option_clear",  "hand_hand_cmd_option_clear_desc");

            addLocalizationIfExists(optionPickUser,     currentLangCode, "hand_pick_cmd_option_user", "hand_pick_cmd_option_desc");
        }

        handCommand.add_option(optionHandNext).add_option(optionHandList).add_option(optionHandRandom).add_option(optionHandClear);
        pickCommand.add_option(optionPickUser);
    }
    
    // hand pick menu
    m_Spec.selectMenuIds.emplace_back(handPickMenuId);

    // user pick context menu
    {
        dpp::slashcommand& command = m_Spec.commands.emplace_back();
        command.set_type(dpp::ctxm_user)
        .set_name("Pick hand")
        .set_description("Pick this person even if their hand isn't raised");
    }

}


// Discord event handlers

void HandsCommand::command(const CommandContext& ctx) {
    dpp::command_interaction ci = ctx.event.command.get_command_interaction();

    const std::string& commandName = ci.name;

    if(commandName == Localization::getString("hand_hand_cmd"))         handCmd(ctx);
    else if(commandName == Localization::getString("hand_raise_cmd"))   raiseCmd(ctx);
    else if(commandName == Localization::getString("hand_lower_cmd"))   lowerCmd(ctx);
    else if(commandName == Localization::getString("hand_pick_cmd"))    pickCmd(ctx);
    else {
        LOG_WARN("Hands: unknown command: " + commandName);
        ctx.replyUnlocalized("Unknown command");
    }
}

void HandsCommand::userContext(const CommandContext& ctx) {
    dpp::user_context_menu_t& event = (dpp::user_context_menu_t&)ctx.event;
    const std::string& commandName = event.command.get_command_name();

    if(commandName == "Pick hand")    pickUserContext(ctx);
    else {
        LOG_WARN("HandsCommand: unknown user context command: " + commandName);
        ctx.replyUnlocalized("Unknown user context menu");
    }
}

void HandsCommand::selectClick(const CommandContext& ctx) {
    dpp::select_click_t& event = (dpp::select_click_t&)ctx.event;

    const std::string& menuId = event.custom_id;

    if(menuId == handPickMenuId)    pickFromList(ctx);
    else {
        LOG_WARN("HandsCommand: unknown select menu id: " + menuId);
        ctx.replyUnlocalized("Unknown select menu");
    }

}


// Helper functions

// send a message in the channel to indicate which user was picked
void pickMessage(const CommandContext& ctx, const dpp::snowflake& userId) {
    dpp::message msg(
        fmt::format(
            ctx.localizeGuild("hand_picked_message"),
            fmt::arg("picker", mentionUser(ctx.userId())),
            fmt::arg("picked", mentionUser(userId))
        )
    );
    msg.allowed_mentions.users.emplace_back(userId);

    ctx.replyUnlocalizedChannel(msg);
}

// reply to the command-issuer with either a confirmation or a warning that the user didn't have their hand up
void replyConfirmOrWarn(const CommandContext& ctx, const dpp::snowflake& userId, const std::list<dpp::snowflake>& hands) {
    bool hadHandUp = false;
    if(hands.size()) {
        for (auto& hand : hands) {
            if(hand == userId) {
                hadHandUp = true;
                break;
            }
        }
    }

    if(hadHandUp) {
        ctx.confirm();
    } else {
        ctx.replyUnlocalized(
            fmt::format(
                ctx.localize("hand_picked_user_without_hand"),
                fmt::arg("picked", mentionUser(userId))
            ),
            true
        );
    }
}

dpp::message createHandPicker(const std::list<dpp::snowflake>& hands, const dpp::snowflake& guildId) {
    dpp::component listComponent = dpp::component()
        .set_type(dpp::cot_selectmenu)
        .set_placeholder("Pick a user") // TODO: Localize
        .set_id("hands_pick_user");

    for(auto userId : hands) {
        listComponent.add_select_option(dpp::select_option(nickOrUsername(userId, guildId), std::to_string(userId)));
    }

    return dpp::message().add_component(dpp::component().add_component(listComponent)).set_flags(dpp::m_ephemeral);
}


// Command handlers

void HandsCommand::handCmd(const CommandContext& ctx) {
    dpp::command_interaction ci = ctx.event.command.get_command_interaction();

    if(ci.options.size() != 1) {
        LOG_WARN("HandsCommand: expected exactly one option, got" + std::to_string(ci.options.size()));
        ctx.event.reply("Expected exactly one option, got " + std::to_string(ci.options.size()));
        return;
    }

    const std::string& subCommand = ci.options[0].name;

    if(subCommand == Localization::getString("hand_hand_cmd_option_next"))          handNext(ctx);
    else if(subCommand == Localization::getString("hand_hand_cmd_option_clear"))    handClear(ctx);
    else if(subCommand == Localization::getString("hand_hand_cmd_option_random"))   handRandom(ctx);
    else if(subCommand == Localization::getString("hand_hand_cmd_option_list"))     handList(ctx);
    else {
        LOG_WARN("HandsCommand: unknown subcommand: " + subCommand);
        ctx.replyUnlocalized("Unknown subcommand");
    }
}

void HandsCommand::raiseCmd(const CommandContext& ctx) {
    auto hands = HandRepo::get(ctx.channelId());

    for(auto hand : hands) {
        auto handUser = hand;
        if(ctx.userId() == handUser) {
            ctx.replyLocalized("hand_already_raised", true);
            return;
        }
    }

    if (hands.size() >= 50) {
        ctx.replyLocalized("hand_limit", true);
        return;
    }

    HandRepo::raise(ctx.channelId(), ctx.userId());
    ctx.confirm(); // TODO: send non-ephemeral message in channel to let everyone know user raised their hand (needs some formatting)
    ctx.replyUnlocalizedChannel(
        fmt::format(
            ctx.localizeGuild("hand_user_raised_hand"),
            fmt::arg("raiser", mentionUser(ctx.userId()))
        )
    );
}

void HandsCommand::lowerCmd(const CommandContext& ctx) {
    HandRepo::lower(ctx.channelId(), ctx.userId());
    ctx.confirm();
}

void HandsCommand::pickCmd(const CommandContext& ctx) {
    // TODO: Add admin check

    dpp::command_interaction ci = ctx.event.command.get_command_interaction();

    auto hands = HandRepo::get(ctx.channelId());

    if(ci.options.size() == 1) {
        // user was selected
        if(!ci.options[0].type == dpp::co_user) {
            ctx.replyUnlocalized("Expected a user as first argument to pick");
            return;
        }
        const dpp::snowflake& userId = std::get<dpp::snowflake>(ci.options[0].value);
        
        HandRepo::lower(ctx.channelId(), userId);
        replyConfirmOrWarn(ctx, userId, hands);
        pickMessage(ctx, userId);
        return;
    }

    if(!hands.size()) {
        ctx.replyLocalized("hand_no_hands", true);
        return;
    }

    ctx.replyUnlocalized(createHandPicker(hands, ctx.guildId()));
}


// hand subcommands

void HandsCommand::handNext(const CommandContext& ctx) {
    // TODO: check if admin

    auto hands = HandRepo::get(ctx.channelId());

    if(!hands.size()) {
        ctx.replyLocalized("hand_no_hands", true);
        return;
    }

    dpp::snowflake userId = hands.front();

    HandRepo::lower(ctx.channelId(), userId);
    ctx.confirm();
    pickMessage(ctx, userId);
}

void HandsCommand::handClear(const CommandContext& ctx) {
    // TODO: check if admin

    HandRepo::clear(ctx.channelId());
    ctx.confirm();
}

void HandsCommand::handRandom(const CommandContext& ctx) {
    // TODO: add admin check

    auto hands = HandRepo::get(ctx.channelId());

    if(!hands.size()) {
        ctx.replyLocalized("hand_no_hands", true);
        return;
    }

    std::srand(std::time(NULL));
    int random = std::rand() % hands.size(); // random number between 0 and size of hands

    auto it = hands.begin();
    while(random) { // go to random element in hands
        it++;
        random--;
    }

    dpp::snowflake userId = *it;

    HandRepo::lower(ctx.channelId(), userId);
    ctx.confirm();
    pickMessage(ctx, userId);
}

void HandsCommand::handList(const CommandContext& ctx) {
    auto hands = HandRepo::get(ctx.channelId());

    if(!hands.size()) {
        ctx.replyLocalized("hand_no_hands", true);
        return;
    }

    std::stringstream ss;
    ss << ctx.localize("hand_up_users") + "\n";

    auto it = hands.begin();
    const auto& guildId = ctx.guildId();
    while (it != hands.end()) {
        ss << mentionUser(*it) << "\n";
        it++;
    }
    ctx.replyUnlocalized(ss.str(), true);
}


// Select menu click handler(s)

void HandsCommand::pickFromList(const CommandContext& ctx) {
    // TODO: add admin check

    dpp::select_click_t& event = (dpp::select_click_t&)ctx.event;
    dpp::snowflake selectedUserId = std::stoull(event.values[0]);
    
    ctx.reply();
    pickMessage(ctx, selectedUserId);

    HandRepo::lower(ctx.channelId(), selectedUserId);

    auto hands = HandRepo::get(ctx.channelId());
    if(!hands.size()) {
        ctx.event.edit_original_response(dpp::message(ctx.localize("hand_no_hands")));
        return;
    }

    ctx.event.edit_original_response(createHandPicker(hands, ctx.guildId()));

}


// Context menu handler(s)

void HandsCommand::pickUserContext(const CommandContext& ctx) {
    dpp::user_context_menu_t& event = (dpp::user_context_menu_t&)ctx.event;

    auto hands = HandRepo::get(ctx.channelId());
    const dpp::snowflake& selectedUser = event.get_user().id;

    replyConfirmOrWarn(ctx, selectedUser, hands);

    HandRepo::lower(ctx.channelId(), selectedUser);

    pickMessage(ctx, selectedUser);
    
}
