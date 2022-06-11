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
    
    {
        // application ids are overwritten by main class when registering commands, set them to NULL for now
        dpp::slashcommand& raiseCommand = m_Spec.commands.emplace_back(Localization::getString("hand_cmd_raise"), Localization::getString("hand_cmd_raise_desc"), NULL);
        
        dpp::slashcommand& lowerCommand = m_Spec.commands.emplace_back(Localization::getString("hand_cmd_lower"), Localization::getString("hand_cmd_lower_desc"), NULL);

        dpp::slashcommand& handsCommand = m_Spec.commands.emplace_back(Localization::getString("hand_cmd_hands"), Localization::getString("hand_cmd_hands_desc"), NULL);
        
        dpp::slashcommand& pickCommand = m_Spec.commands.emplace_back(Localization::getString("hand_cmd_pick"), Localization::getString("hand_cmd_pick_desc"), NULL)
            .set_default_permissions(dpp::p_manage_messages);
            dpp::command_option optionPickNext(dpp::co_sub_command, Localization::getString("hand_cmd_pick_option_next"), Localization::getString("hand_cmd_pick_option_next_desc"));
            dpp::command_option optionPickUser(dpp::co_sub_command, Localization::getString("hand_cmd_pick_option_user"), Localization::getString("hand_cmd_pick_option_user_desc"));
                dpp::command_option optionPickUserUser(dpp::co_user, Localization::getString("hand_cmd_pick_option_user"), Localization::getString("hand_cmd_pick_option_user_desc"));
            dpp::command_option optionPickList(dpp::co_sub_command, Localization::getString("hand_cmd_pick_option_list"), Localization::getString("hand_cmd_pick_option_list_desc"));
            dpp::command_option optionPickRandom(dpp::co_sub_command, Localization::getString("hand_cmd_pick_option_random"), Localization::getString("hand_cmd_pick_option_random_desc"));
            dpp::command_option optionPickClear(dpp::co_sub_command, Localization::getString("hand_cmd_pick_option_clear"), Localization::getString("hand_cmd_pick_option_clear_desc"));

        for(const auto& lang : Localization::getLanguages()) {
            const std::string& currentLangCode = lang.first;

            if(currentLangCode == "en-US") continue;

            addLocalizationIfExists(raiseCommand, currentLangCode, "hand_cmd_raise", "hand_cmd_raise_desc");
            
            addLocalizationIfExists(lowerCommand, currentLangCode, "hand_cmd_lower", "hand_cmd_lower_desc");
            
            addLocalizationIfExists(pickCommand, currentLangCode, "hand_cmd_pick",  "hand_cmd_pick_desc");
                addLocalizationIfExists(optionPickNext, currentLangCode, "hand_cmd_pick_option_next", "hand_cmd_pick_option_next_desc");
                addLocalizationIfExists(optionPickList, currentLangCode, "hand_cmd_pick_option_list", "hand_cmd_pick_option_list_desc");
                addLocalizationIfExists(optionPickUser, currentLangCode, "hand_cmd_pick_option_user", "hand_cmd_pick_option_user_desc");
                    addLocalizationIfExists(optionPickUserUser, currentLangCode, "hand_cmd_pick_option_user", "hand_cmd_pick_option_user_desc");
                addLocalizationIfExists(optionPickRandom, currentLangCode, "hand_cmd_pick_option_random", "hand_cmd_pick_option_random_desc");

            addLocalizationIfExists(handsCommand, currentLangCode, "hand_cmd_hands", "hand_cmd_hands_desc");

        }
        
        pickCommand
            .add_option(optionPickNext)
            .add_option(optionPickUser
                .add_option(optionPickUserUser))
            .add_option(optionPickList)
            .add_option(optionPickRandom)
            .add_option(optionPickClear);

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

    if(commandName == Localization::getString("hand_cmd_raise"))        raiseCmd(ctx);
    else if(commandName == Localization::getString("hand_cmd_lower"))   lowerCmd(ctx);
    else if(commandName == Localization::getString("hand_cmd_hands"))   handsCmd(ctx);
    else if(commandName == Localization::getString("hand_cmd_pick"))    pickCmd(ctx);
    else {
        LOG_WARN("Hands: unknown command: " + commandName);
        ctx.replyUnlocalized("Unknown command", true);
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
void pickMessage(const CommandContext& ctx, const dpp::snowflake& userId, bool wasRaised = false) {
    dpp::message msg(
        fmt::format(
            ctx.localizeGuild(wasRaised ? "hand_picked_message" : "hand_picked_message_not_raised"),
            fmt::arg("picker", mentionUser(ctx.userId())),
            fmt::arg("picked", mentionUser(userId))
        )
    );
    msg.allowed_mentions.users.emplace_back(userId);

    ctx.replyUnlocalized(msg);
}

bool wasRaised(const dpp::snowflake& userId, const std::list<dpp::snowflake>& hands) {
    if(hands.size()) {
        for (auto& hand : hands) {
            if(hand == userId) {
                return true;
                break;
            }
        }
    }

    return false;
}

dpp::message createHandPicker(const CommandContext& ctx, const std::list<dpp::snowflake>& hands, const dpp::snowflake& guildId) {
    dpp::component listComponent = dpp::component()
        .set_type(dpp::cot_selectmenu)
        .set_placeholder(ctx.localize("hand_menu_placeholder"))
        .set_id("hands_pick_user");

    for(auto userId : hands) {
        listComponent.add_select_option(dpp::select_option(nickOrUsername(userId, guildId), std::to_string(userId)));
    }

    return dpp::message().add_component(dpp::component().add_component(listComponent)).set_flags(dpp::m_ephemeral);
}


// Command handlers

void HandsCommand::handsCmd(const CommandContext& ctx) {
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
    ctx.replyUnlocalized(
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
    dpp::command_interaction ci = ctx.event.command.get_command_interaction();

    const std::string& subCommand = ci.options[0].name;

    if(subCommand == Localization::getString("hand_cmd_pick_option_next"))          pickNext(ctx);
    else if(subCommand == Localization::getString("hand_cmd_pick_option_list"))     pickList(ctx);
    else if(subCommand == Localization::getString("hand_cmd_pick_option_user"))     pickUser(ctx);
    else if(subCommand == Localization::getString("hand_cmd_pick_option_random"))   pickRandom(ctx);
    else if(subCommand == Localization::getString("hand_cmd_pick_option_clear"))    pickClear(ctx);
    else {
        LOG_WARN("HandsCommand: unknown subcommand: " + subCommand);
        ctx.replyUnlocalized("Unknown subcommand");
    }
}


// pick subcommands

void HandsCommand::pickNext(const CommandContext& ctx) {
    auto hands = HandRepo::get(ctx.channelId());

    if(!hands.size()) {
        ctx.replyLocalized("hand_no_hands", true);
        return;
    }

    dpp::snowflake userId = hands.front();

    HandRepo::lower(ctx.channelId(), userId);
    pickMessage(ctx, userId);
}

void HandsCommand::pickUser(const CommandContext& ctx) {
    dpp::command_interaction ci = ctx.event.command.get_command_interaction();

    auto hands = HandRepo::get(ctx.channelId());

    if(ci.options[0].options[0].type != dpp::co_user) {
        ctx.replyUnlocalized("Expected a user as first argument to pick");
        return;
    }

    const dpp::snowflake& userId = std::get<dpp::snowflake>(ci.options[0].options[0].value);
    
    HandRepo::lower(ctx.channelId(), userId);
    pickMessage(ctx, userId, wasRaised(userId, hands));
}

void HandsCommand::pickList(const CommandContext& ctx) {
    dpp::command_interaction ci = ctx.event.command.get_command_interaction();

    auto hands = HandRepo::get(ctx.channelId());

    if(!hands.size()) {
        ctx.replyLocalized("hand_no_hands", true);
        return;
    }

    ctx.replyUnlocalized(createHandPicker(ctx, hands, ctx.guildId()));
}

void HandsCommand::pickRandom(const CommandContext& ctx) {
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
    pickMessage(ctx, userId);
}

void HandsCommand::pickClear(const CommandContext& ctx) {
    HandRepo::clear(ctx.channelId());
    ctx.replyUnlocalized(
        fmt::format(
            ctx.localizeGuild("hand_cleared_hands"),
            fmt::arg("clearer", mentionUser(ctx.userId()))
        )
    );
}

// Select menu click handler(s)

void HandsCommand::pickFromList(const CommandContext& ctx) {
    // TODO: Potentially unsafe cast
    dpp::select_click_t& event = (dpp::select_click_t&)ctx.event;
    dpp::snowflake selectedUserId = std::stoull(event.values[0]);
    
    pickMessage(ctx, selectedUserId);

    HandRepo::lower(ctx.channelId(), selectedUserId);

    auto hands = HandRepo::get(ctx.channelId());
    if(!hands.size()) {
        ctx.event.edit_original_response(dpp::message(ctx.localize("hand_no_hands")));
        return;
    }

    ctx.event.edit_original_response(createHandPicker(ctx, hands, ctx.guildId()));
}

// Context menu handler(s)

void HandsCommand::pickUserContext(const CommandContext& ctx) {
    dpp::user_context_menu_t& event = (dpp::user_context_menu_t&)ctx.event;

    auto hands = HandRepo::get(ctx.channelId());
    const dpp::snowflake& selectedUser = event.get_user().id;

    HandRepo::lower(ctx.channelId(), selectedUser);

    pickMessage(ctx, selectedUser, wasRaised(selectedUser, hands));
}
