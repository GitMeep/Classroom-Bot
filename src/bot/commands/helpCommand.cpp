#include "helpCommand.h"
#include "../bot.h"

void HelpCommand::call(std::vector<std::string> parameters, MessageInfo current) {
    Command::call(parameters, current);

    std::string commandName = "";
    if (parameters.size()) {
        commandName = parameters[0];
    }

    std::vector<CommandInfo> commands = _bot->_commandHandler.getInfo(commandName);
    if (!commands.size()) {
        _aegisCore->create_message(_current.channelId, "Unknown command.");
        return;
    }

    nlohmann::json embed;

    bool detail = commandName != "";
    if (detail) {
        if(!_current.isDm) {
            _aegisCore->create_message(current.channelId, "Help about a specific command is only supported in DM's.");
            return;
        }
        CommandInfo command = commands[0];
        embed["title"] = command.name;
        embed["description"] = command.description;

        if (command.options.size()) {
            std::string optionsString;
            auto option = command.options.begin();
            while (option != command.options.end()) {
                optionsString += *option + "\n";
                option++;
            }
            embed["fields"].push_back({{"name", "Options"}, {"value", optionsString}});
        }

        if (command.aliases.size()) {
            std::string aliases;
            auto alias = command.aliases.begin();
            while (alias != command.aliases.end()) {
                aliases += *alias;
                alias++;
                if(alias != command.aliases.end())
                    aliases += " | ";
            }
            embed["fields"].push_back({{"name", "Aliases"}, {"value", aliases}});
        }

    } else {
        embed["title"] = "Commands";
        embed["description"] = "To use the admin only commands, you need a role named \"Teacher\". The name of this role can be changed with the `settings` command. If you have any questions or want to report a bug, join the [support server](https://discord.gg/dqmTAZY). If you like the bot, please upvote it on [top.gg](https://top.gg/bot/691945666896855072)";
        auto command = commands.begin();
        while (command != commands.end()) {
            std::string commandName = command->name;
            auto alias = command->aliases.begin();
            while (alias != command->aliases.end()) {
                commandName += " | " + *alias;
                alias++;
            }
            embed["fields"].push_back({{"name", "`" + commandName + "`"}, {"value", command->description}});
            command++;
        }
    }

    _aegisCore->create_dm_message({current.userId, "", embed});
}

bool HelpCommand::checkPermissions(aegis::permission channelPermissions) {
    return true;
}

CommandInfo HelpCommand::getCommandInfo() {
    return {
        "help",
        {"he"},
        "Display this help page. Use `help` [command] to get information about a specific command.",
        {
            "`[command]` to get help about a specific command."
        }
    };
}
