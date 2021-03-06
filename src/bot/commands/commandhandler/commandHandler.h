#pragma once

#include <bot/commands/command.h>

class CommandHandler {
public:
    bool parseAndCall(const std::string& input, CommandContext* ctx);
    bool callCommand(const std::string& name, const std::vector<std::string>& parameters, CommandContext* ctx);
    void registerCommand(Command* command);
    std::vector<CommandInfo> getInfo(const std::string& commandName);
    std::string getUnlocalizedName(std::string lang, const std::string& localizedCommand);

private:
    std::unordered_map<std::string, std::shared_ptr<Command>> m_Commands; // command name -> command class
    std::unordered_map<std::string, std::string> m_Aliases; // alias -> command name
    std::unordered_map<std::string, CommandInfo> m_CommandInfos; // command name -> info
};