#include <cbpch.h>

#include <bot/commands/handsCommand.h>
#include <bot/localization/localization.h>
#include <bot/utils/utils.h>
#include <bot/bot.h>
#include <bot/persistence/repo/handRepo.h>

const std::string actionMsg = "Please enter a valid action. Options are: ```up, down, next, pick, random, list, clear```";

void HandsCommand::call(int verb, const std::vector<std::string>& parameters, CommandContext* ctx) {
    switch(verb) {
    case 1: // up
        up(ctx);
        break;
    case 2: // down
        down(ctx);
        break;
    case 3: // list
        list(ctx);
        break;
    case 4: // next
        next(ctx);
        break;
    case 5: // pick
        pick(ctx, parameters);
        break;
    case 6: // random
        random(ctx);
        break;
    case 7: // clear
        clear(ctx);
        break;
    default:
    case 0: // no option was entered
        ctx->respond("hand_usage");
        break;
    }
}

void HandsCommand::up(CommandContext* ctx) {
    auto hands = m_Bot->getHandRepo()->get(ctx->getChannelId());

    for(auto hand : hands) {
        auto handUser = hand;
        if(ctx->getUserId().get() == handUser) {
            ctx->respond("already_raised");
            return;
        }
    }

    if (hands.size() >= 50) {
        ctx->respond("hand_limit");
        return;
    }

    m_Bot->getHandRepo()->raise(ctx->getChannelId(), ctx->getUserId());
    ctx->confirm();
}

void HandsCommand::down(CommandContext* ctx) {
    m_Bot->getHandRepo()->lower(ctx->getChannelId(), ctx->getUserId());
    ctx->confirm();
}

void HandsCommand::next(CommandContext* ctx) {
    if(!ctx->isAdmin()) {
        ctx->respond("admin_required");
        return;
    }

    auto hands = m_Bot->getHandRepo()->get(ctx->getChannelId());

    if(!hands.size()) {
        ctx->respond("no_hands");
        return;
    }

    aegis::snowflake user = hands.front();
    std::string username = getUsername(user, ctx->getGuildId());

    m_Bot->getHandRepo()->lower(ctx->getChannelId(), user);
    ctx->respondUnlocalized("```" + username + "```");
}

void HandsCommand::clear(CommandContext* ctx) {
    if(!ctx->isAdmin()) {
        ctx->respond("admin_required");
        return;
    }

    m_Bot->getHandRepo()->clear(ctx->getChannelId());
    ctx->confirm();
}

void HandsCommand::list(CommandContext* ctx) {
    auto hands = m_Bot->getHandRepo()->get(ctx->getChannelId());

    if(!hands.size()) {
        ctx->respond("no_hands");
        return;
    }
    std::stringstream ss;
    ss << ClassroomBot::get().getLocalization()->getString("eng", "hand_up_users") + "\n```";

    int number = 1;
    auto it = hands.begin();
    auto guildId = ctx->getGuildId();
    while (it != hands.end()) {
        ss << number << ": " << getUsername(*it, guildId) << "\n";
        it++;
        number++;
    }
    ss << "```\n";
    ctx->respondUnlocalized(ss.str());
}

void HandsCommand::random(CommandContext* ctx) {
    if(!ctx->isAdmin()) {
        ctx->respond("admin_required");
        return;
    }

    auto hands = m_Bot->getHandRepo()->get(ctx->getChannelId());

    if(!hands.size()) {
        ctx->respond("no_hands");
        return;
    }

    std::srand(std::time(NULL));
    int random = std::rand() % hands.size(); // random number between 0 and size of hands

    auto it = hands.begin();
    while(random) { // go to random element in hands
        it++;
        random--;
    }

    aegis::snowflake user = *it;
    std::string username = getUsername(user, ctx->getGuildId());

    m_Bot->getHandRepo()->lower(ctx->getChannelId(), user);

    ctx->respondUnlocalized("```" + username + "```");
}

void HandsCommand::pick(CommandContext* ctx, const std::vector<std::string>& parameters) {
    if(!ctx->isAdmin()) {
        ctx->respond("admin_required");
        return;
    }

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

    auto hands = m_Bot->getHandRepo()->get(ctx->getChannelId());

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

    aegis::snowflake user = *it;
    std::string username = getUsername(user, ctx->getGuildId());
    
    m_Bot->getHandRepo()->lower(ctx->getChannelId(), user);

    ctx->respondUnlocalized("```" + username + "```");
}

CommandInfo HandsCommand::getCommandInfo() {
    return {
        "hand",
        "hand_cmd",
        {"hand_alias"},
        "hand_desc",
        {
            "hand_option_up_desc",
            "hand_option_down_desc",
            "hand_option_list_desc",
            "hand_option_next_desc",
            "hand_option_pick_desc",
            "hand_option_random_desc",
            "hand_option_clear_desc"
        },
        {
            "hand_option_up",
            "hand_option_down",
            "hand_option_list",
            "hand_option_next",
            "hand_option_pick",
            "hand_option_random",
            "hand_option_clear"
        },
        true
    };
}