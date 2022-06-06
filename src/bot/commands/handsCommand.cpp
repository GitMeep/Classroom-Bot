#include <bot/commands/handsCommand.h>
#include <bot/localization/localization.h>
#include <bot/bot.h>
#include <bot/persistence/repo/handRepo.h>
#include <bot/util/util.h>

const std::string actionMsg = "Please enter a valid action. Options are: ```up, down, next, pick, random, list, clear```";

HandsCommand::HandsCommand() :
Command::Command() {
    
    // /hand command
    {
        dpp::command_option optionUp(dpp::co_sub_command, Localization::getString("hand_option_up"), Localization::getString("hand_option_up_desc"));
        dpp::command_option optionDown(dpp::co_sub_command, Localization::getString("hand_option_down"), Localization::getString("hand_option_down_desc"));
        dpp::command_option optionNext(dpp::co_sub_command, Localization::getString("hand_option_next"), Localization::getString("hand_option_next_desc"));
        dpp::command_option optionPick(dpp::co_sub_command, Localization::getString("hand_option_pick"), Localization::getString("hand_option_pick_desc"));
        dpp::command_option optionRandom(dpp::co_sub_command, Localization::getString("hand_option_random"), Localization::getString("hand_option_random_desc"));
        dpp::command_option optionClear(dpp::co_sub_command, Localization::getString("hand_option_clear"), Localization::getString("hand_option_clear_desc"));

        optionPick.add_option(dpp::command_option(dpp::co_user, "user", "Select user"));

        dpp::slashcommand& command = m_Spec.commands.emplace_back();

        for(auto lang : Localization::getLanguages()) {
            const std::string& currentLangCode = lang.first;

            if(currentLangCode == "en-US") continue;

            command.add_localization(currentLangCode, Localization::getString("hand_cmd", currentLangCode), Localization::getString("hand_desc", currentLangCode));
            optionUp.add_localization(currentLangCode, Localization::getString("hand_option_up", currentLangCode), Localization::getString("hand_option_up_desc", currentLangCode));
            optionDown.add_localization(currentLangCode, Localization::getString("hand_option_down", currentLangCode), Localization::getString("hand_option_down_desc", currentLangCode));
            optionNext.add_localization(currentLangCode, Localization::getString("hand_option_next", currentLangCode), Localization::getString("hand_option_next_desc", currentLangCode));
            optionPick.add_localization(currentLangCode, Localization::getString("hand_option_pick", currentLangCode), Localization::getString("hand_option_pick_desc", currentLangCode));
            optionRandom.add_localization(currentLangCode, Localization::getString("hand_option_random", currentLangCode), Localization::getString("hand_option_random_desc", currentLangCode));
            optionClear.add_localization(currentLangCode, Localization::getString("hand_option_clear", currentLangCode), Localization::getString("hand_option_clear_desc", currentLangCode));
        }

        command.set_name(Localization::getString("hand_cmd"))
        .set_description(Localization::getString("hand_desc"))
        .set_dm_permission(false)

        .add_option(optionUp)
        .add_option(optionDown)
        .add_option(optionNext)
        .add_option(optionPick)
        .add_option(optionRandom)
        .add_option(optionClear);
    }
    
    // hand pick menu
    m_Spec.selectMenuIds.emplace_back("hands_pick_user");

    // user pick context menu
    {
        dpp::slashcommand& command = m_Spec.commands.emplace_back();
        command.set_type(dpp::ctxm_user)
        .set_name("Pick")
        .set_description("Pick this person even if their hand isn't raised");
    }

}

void HandsCommand::command(const CommandContext& ctx) {
    dpp::command_interaction ci = ctx.event.command.get_command_interaction();

    if(ci.options.size() != 1) {
        LOG_WARN("HandsCommand: expected exactly one option, got" + std::to_string(ci.options.size()));
        ctx.event.reply("Expected exactly one option, got" + std::to_string(ci.options.size()));
        return;
    }

    const std::string& subCommand = ci.options[0].name;

    if(subCommand == "up")          up(ctx);
    else if(subCommand == "down")   down(ctx);
    else if(subCommand == "next")   next(ctx);
    else if(subCommand == "clear")  clear(ctx);
    else if(subCommand == "random") random(ctx);
    else if(subCommand == "pick")   pick(ctx);
    else {
        LOG_WARN("HandsCommand: unknown subcommand: " + subCommand);
        ctx.event.reply("Unknown subcommand");
    }
}

void HandsCommand::userContext(const CommandContext& ctx) {

}

void HandsCommand::messageContext(const CommandContext& ctx) {

}

void HandsCommand::buttonClick(const CommandContext& ctx) {

}

void HandsCommand::selectClick(const CommandContext& ctx) {

}

void HandsCommand::formSubmit(const CommandContext& ctx) {

}

void HandsCommand::up(const CommandContext& ctx) {
    auto hands = HandRepo::get(ctx.channelId());

    for(auto hand : hands) {
        auto handUser = hand;
        if(ctx.userId() == handUser) {
            ctx.replyLocalized("already_raised", true);
            return;
        }
    }

    if (hands.size() >= 50) {
        ctx.replyLocalized("hand_limit", true);
        return;
    }

    HandRepo::raise(ctx.channelId(), ctx.userId());
    ctx.confirm(); // TODO: send non-ephemeral message in channel to let everyone know user raised their hand (needs some formatting)
}

void HandsCommand::down(const CommandContext& ctx) {
    HandRepo::lower(ctx.channelId(), ctx.userId());
    ctx.confirm();
}

void HandsCommand::next(const CommandContext& ctx) {
    // TODO: check if admin

    auto hands = HandRepo::get(ctx.channelId());

    if(!hands.size()) {
        ctx.replyLocalized("no_hands", true);
        return;
    }

    dpp::snowflake userId = hands.front();

    HandRepo::lower(ctx.channelId(), userId);
    ctx.replyUnlocalized(mentionUser(userId) + " was selected");
}

void HandsCommand::clear(const CommandContext& ctx) {
    // TODO: check if admin

    HandRepo::clear(ctx.channelId());
    ctx.confirm();
}

void HandsCommand::random(const CommandContext& ctx) {
    // TODO: add admin check

    auto hands = HandRepo::get(ctx.channelId());

    if(!hands.size()) {
        ctx.replyLocalized("no_hands", true);
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

    ctx.replyUnlocalized(mentionUser(userId) + " was selected"); // TODO: add localization for this
}

void HandsCommand::pickList(const CommandContext& ctx) {
    ctx.replyUnlocalized("Not implemented yet", true);
}

void HandsCommand::pick(const CommandContext& ctx) {
    // TODO: Add admin check

    ctx.replyUnlocalized("Not implemented yet", true);

    /*
    dpp::command_interaction ci = event.command.get_command_interaction();

    if (parameters.size() < 2) {
        ctx->respond("pick_number");
        return;
    }
    
    int number;
    try {
        number = std::stoi(parameters[1]);
    } catch(std::invalid_argument) {
        ctx->respond("valid_number");
        return;
    } catch(std::out_of_range) {
        ctx->respond("reasonable_number");
        return;
    }

    auto hands = HandRepo::get(ctx->getChannelId());

    if(!hands.size()) {
        ctx->respond("no_hands");
        return;
    }

    if(number > hands.size()) {
        ctx->respond("hand_oor");
        return;
    }

    if(number < 1) {
        ctx->respond("above_zero");
        return;
    }

    number--; // start at 0
    auto it = hands.begin();
    while(number) {
        it++;
        number--;
    }

    dpp::snowflake userId = *it;
    
    HandRepo::lower(ctx->getChannelId(), userId);

    event.reply("<@" + std::to_string(userId) + "> was selected");
    */
}