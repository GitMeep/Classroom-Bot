#include <cbpch.h>

#include <bot/bot.h>
#include <bot/commands/commandhandler/commandHandler.h>
#include <bot/localization/localization.h>

/**
 * Parses a command from a string input and information about the messsage, and calls it.
 * 
 * @param input The raw message from Discord to parse
 * @param messageInfo The message info object containing the author, channel, guild, etc.
 * @returns True, if command was found and called, false otherwise
*/
bool CommandHandler::parseAndCall(const std::string& input, CommandContext* ctx) {
    std::vector<std::string> parameters;
    std::stringstream ss;
    ss << input;

    std::string commandName;
    ss >> commandName;

    if(commandName == "") return true; // empty command, do nothing

    std::string param;
    while(ss >> param) {
        parameters.emplace_back(param);
    }

    return callCommand(commandName, parameters, ctx);
}

/**
 * Call a command by it's name, parameters and message info
 * 
 * @param name Name of the command
 * @param parameters The paramters to pass to the command
 * @param messageInfo Message info object
 * @returns true, if the command was found, false if not
 */
bool CommandHandler::callCommand(const std::string& name, const std::vector<std::string>& parameters, CommandContext* ctx) {
    std::string commandName = name;
    std::transform(commandName.begin(), commandName.end(), commandName.begin(), [](unsigned char c){ return std::tolower(c); }); // make sure name is lowercase

    if(commandName[0] == '?') return true; // someone wrote something like "????"", don't do anything
    
    if(commandName != "help") { // ?help should always work
        commandName = getUnlocalizedName(ctx->getSettings().lang, commandName);
    }

    if (m_Aliases.count(name)) {
        commandName = m_Aliases[name];
    }

    LocHelper loc(ClassroomBot::get().getLocalization(), ctx->getSettings().lang);

    if(!m_Commands.count(commandName)) return false;
    if(m_CommandInfos[commandName].noDM && ctx->isDM()) {
        ctx->respond("no_dm");
        return true;
    }

    int verb = 0;
    auto options = m_CommandInfos[commandName].options;
    if(parameters.size() > 0 && options.size() > 0) {
        for(int i = 0; i < options.size(); i++) {
            if(parameters[0] == loc.get(options[i])) {
                verb = i+1;
                break;
            }
        }
    }

    m_Commands[commandName]->call(verb, parameters, ctx);
    return true;
}

/**
 * Register a new command
 * 
 * @param command Pointer to the command
*/
void CommandHandler::registerCommand(Command* command) {
    CommandInfo info = command->getCommandInfo();

    if (m_Commands.count(info.name)) return; // command already exists
    if (m_Aliases.count(info.name)) return; // command already exists

    m_Commands[info.name] = std::shared_ptr<Command>(command);
    m_CommandInfos[info.name] = info;

    auto alias = info.aliases.begin();
    while (alias != info.aliases.end()) {
        m_Aliases[*alias] = info.name;
        alias++;
    }
    
}

std::vector<CommandInfo> CommandHandler::getInfo(const std::string& commandName) {
    std::vector<CommandInfo> info;
    if(commandName == "") {
        auto help = m_CommandInfos.begin();
        while(help != m_CommandInfos.end()) {
            info.emplace_back(help->second);
            help++;
        }
        return info;
    }

    std::string name = commandName;
    if (m_Aliases.count(name)) {
        name = m_Aliases[commandName];
    }

    if(m_CommandInfos.count(name)) {
        info.emplace_back(m_CommandInfos[name]);
        return info;
    }

    return info;
}

std::string CommandHandler::getUnlocalizedName(std::string lang, const std::string& localizedCommand) {
    if(lang == "") lang = "eng";
    auto loc = ClassroomBot::get().getLocalization();
    auto info = m_CommandInfos.begin();
    while(info != m_CommandInfos.end()) {
        if(loc->getString(lang, info->second.localName) == localizedCommand) {
            return info->second.name;
        }
        auto alias = info->second.aliases.begin();
        while(alias != info->second.aliases.end()) {
            if(loc->getString(lang, *alias) == localizedCommand) {
                return info->second.name;
            }
            alias++;
        }
        info++;
    }

    return "--"; // should result in an unkown command
}
