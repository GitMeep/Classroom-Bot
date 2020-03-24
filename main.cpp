#include <aegis.hpp>
using json = nlohmann::json;
int main(int argc, char * argv[])
{
    // Create bot object with a minimum log level of trace
    aegis::core bot(aegis::create_bot_t().log_level(spdlog::level::trace).token("NjkxOTQ1NjY2ODk2ODU1MDcy.Xnnqxw.kY6D6U2v7MrRXgmZltV__2w45kw"));
    AEGIS_TRACE(bot.log, "Bot object created");
    // With min log level set to trace and wsdbg (websocket debug) set to true
    // the bot will dump all websocket messages to console
    bot.wsdbg = true;
    // These callbacks are what are called when websocket events occur
    bot.set_on_message_create([&](aegis::gateway::events::message_create obj)
    {
        std::string content{ obj.msg.get_content() };
        auto & _channel = obj.msg.get_channel();
        // Simple Hi response
        if (content == "~Hi")
        {
            _channel.create_message("Hello back");
        }
    });
    // start the bot
    bot.run();
    // yield thread execution to the library
    bot.yield();
    std::cout << "Press any key to continue...\n";
    std::cin.ignore();
    return 0;
}