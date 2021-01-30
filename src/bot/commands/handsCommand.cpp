#include <cbpch.h>

#include <bot/commands/handsCommand.h>
#include <bot/localization/localization.h>
#include <bot/utils/utils.h>
#include <bot/bot.h>
#include <bot/persistence/repo/handRepo.h>

const std::string actionMsg = "Please enter a valid action. Options are: ```up, down, next, pick, random, list, clear```";

void HandsCommand::call(const std::vector<std::string>& parameters, CommandContext* ctx) {
    
    if(ctx->isDM()) {
        ctx->respond("no_dm");
        return;
    }

    if(parameters.size() == 0) {
        ctx->respond("hand_usage");
        return;
    }

    std::string verb = parameters[0];
    if(verb == "up") {
        up(ctx);
    }

    else if(verb == "down") {
        down(ctx);
    }

    else if(verb == "list") {
        list(ctx);
    }

    else if(verb == "next") {
        if(!ctx->isAdmin()) {
            ctx->respond("admin_required");
            return;
        } else {
            next(ctx);
        }
    }

    else if(verb == "random") {
        if(!ctx->isAdmin()) {
            ctx->respond("admin_required");
            return;
        } else {
            random(ctx);
        }
    }

    else if(verb == "clear") {
        if(!ctx->isAdmin()) {
            ctx->respond("admin_required");
            return;
        } else {
            clear(ctx);
        }
    } else if(verb == "pick") {
        if(!ctx->isAdmin()) {
            ctx->respond("admin_required");
            return;
        } else {
            if (parameters.size() < 2) {
                ctx->respond("pick_number");
                return;
            }
            int number;
            try {
                number = std::stoi(parameters[1]);
            } catch(std::invalid_argument) {
                ctx->respond("valid_number");
            } catch(std::out_of_range) {
                ctx->respond("reasonable_number");
            }
            pick(ctx, number);
        }
    } else {
        ctx->respond("hand_usage");
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
    ss << ClassroomBot::get().getLocalization()->getString("eng", "hand_up_users") + ":```";

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

void HandsCommand::pick(CommandContext* ctx, int number) {
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
        {"h"},
        "Show of hands. Raise or lower your hand to indicate a question or an answer to one. The hands are stored per-channel.",
        {
            "up: raise your hand",
            "down: lower your hand",
            "list: list all users with their hand raised",
            "next: (admin only) show the next user with a raised hand, and lower it",
            "pick [number]: (admin only) pick a user from the list",
            "random: (admin only) pick a random user with their hand raised, and lower it",
            "clear: (admin only) lower all hands"
        }
    };
}