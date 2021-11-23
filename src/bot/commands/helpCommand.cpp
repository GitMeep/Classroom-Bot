#include <bot/commands/helpCommand.h>
#include <bot/bot.h>

#include <bot/commands/commandhandler/commandHandler.h>
#include <bot/localization/localization.h>

void HelpCommand::call(int verb, const std::vector<std::string>& parameters, CommandContext* ctx) {
    std::string commandName = "";
    if (parameters.size()) {
        std::string localizedName = parameters[0];
        commandName = m_Bot->getCommandHandler()->getUnlocalizedName(ctx->getSettings().lang, localizedName);
    }

    std::vector<CommandInfo> commands = m_Bot->getCommandHandler()->getInfo(commandName);
    if (!commands.size()) {
        ctx->respond("unknown_cmd");
        return;
    }

    nlohmann::json embed;
    LocHelper loc(ClassroomBot::getLocalization(), ctx->getSettings().lang);

    bool detail = commandName != "";
    if (detail) {
        CommandInfo command = commands[0];
        embed["title"] = loc.get(command.localName);
        embed["description"] = loc.get(command.description);

        if (command.optionDescriptions.size()) {
            std::string optionsString;
            auto option = command.optionDescriptions.begin();
            while (option != command.optionDescriptions.end()) {
                optionsString += loc.get(*option) + "\n";
                option++;
            }
            embed["fields"].push_back({{"name", loc.get("help_options")}, {"value", optionsString}});
        }

        if (command.aliases.size()) {
            std::string aliases;
            auto alias = command.aliases.begin();
            while (alias != command.aliases.end()) {
                aliases += loc.get(*alias);
                alias++;
                if(alias != command.aliases.end())
                    aliases += " | ";
            }
            embed["fields"].push_back({{"name", loc.get("help_aliases")}, {"value", aliases}});
        }

    } else {
        embed["title"] = loc.get("help_response_title");
        embed["description"] = loc.get("help_response_description");
        auto command = commands.begin();
        
        std::string alisesString;
        while (command != commands.end()) {
            alisesString = loc.get(command->localName);
            auto alias = command->aliases.begin();
            while (alias != command->aliases.end()) {
                alisesString += " | " + loc.get(*alias);
                alias++;
            }
            embed["fields"].push_back({{"name", "`" + alisesString + "`"}, {"value", loc.get(command->description)}});
            command++;
        }
    }

    aegis::create_message_t msg;
    msg.content("");
    msg.embed(embed);
    msg.user_id(ctx->getUserId());
    m_Cluster->create_dm_message(msg);
}

CommandInfo HelpCommand::getCommandInfo() {
    return {
        "help",
        "help_cmd",
        {"help_alias"},
        "help_desc",
        {
            "help_option_desc"
        },
        {},
        false
    };
}
