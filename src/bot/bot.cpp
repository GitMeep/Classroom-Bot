#include <Poco/Net/HTTPRequest.h>
#include <Poco/Net/HTTPResponse.h>
#include <Poco/Net/HTTPSClientSession.h>
#include <bot/bot.h>
#include <bot/commands/command.h>
#include <bot/commands/manage.h>
#include <bot/config/config.h>
#include <bot/localization/localization.h>
#include <bot/persistence/db.h>
#include <bot/persistence/repo/handRepo.h>
#include <bot/persistence/repo/muteRepo.h>
#include <bot/persistence/repo/questionRepo.h>
#include <bot/persistence/repo/settingsRepo.h>
#include <dpp/dpp.h>
#include <spdlog/async.h>
#include <spdlog/sinks/rotating_file_sink.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/spdlog.h>

#include <iostream>
#include <vector>

enum ParserException {
  NOT_A_COMMAND,
  EMPTY_COMMAND
};

// initialize static members
dpp::cluster*                         ClassroomBot::m_Cluster;
std::chrono::system_clock::time_point ClassroomBot::m_StartupTime;
unsigned char                         ClassroomBot::m_PresenceState  = 0;
bool                                  ClassroomBot::m_Initialized    = false;
bool                                  ClassroomBot::m_LogInitialized = false;
std::vector<Command*>                 ClassroomBot::m_Commands;
Command*                              ClassroomBot::m_ManageCommand;
std::shared_ptr<spdlog::logger>       ClassroomBot::m_Logger;

void ClassroomBot::init() {
  m_StartupTime = std::chrono::system_clock::now();

  spdlog::init_thread_pool(8192, 2);
  std::vector<spdlog::sink_ptr> sinks;
  auto                          stdout_sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
  auto                          rotating    = std::make_shared<spdlog::sinks::rotating_file_sink_mt>("classroombot.log", 1024 * 1024 * 5, 10);
  sinks.push_back(stdout_sink);
  sinks.push_back(rotating);
  m_Logger = std::make_shared<spdlog::async_logger>("logs", sinks.begin(), sinks.end(), spdlog::thread_pool(), spdlog::async_overflow_policy::block);
  spdlog::register_logger(m_Logger);
  m_Logger->set_pattern("%^%Y-%m-%d %H:%M:%S.%e [%L] [ClassroomBot] [th#%t]%$ : %v");

#ifdef DEBUG
  m_Logger->set_level(spdlog::level::level_enum::trace);
#else
  m_Logger->set_level(spdlog::level::level_enum::info);
#endif

  m_LogInitialized = true;

  log(dpp::ll_info, "Starting ClassroomBot version " + std::string(BOT_VERSION));

  try {
    Config::loadFromFile("config.json");
  } catch (std::runtime_error& e) {
    LOG_ERROR(std::string(e.what()));
    return;
  }

  if (!Config::isLoaded()) {
    throw std::runtime_error("Invalid config supplied to ClassroomBot!");
  }

  std::string token = Config::get()["bot"]["token"];
  m_Cluster         = new dpp::cluster(token);
  m_Cluster->on_log(ClassroomBot::onLog);

  Localization::init();
  DB::init();

  m_Cluster->on_slashcommand(ClassroomBot::onSlashCommand);
  m_Cluster->on_select_click(ClassroomBot::onSelectClick);
  m_Cluster->on_button_click(ClassroomBot::onButtonClick);
  m_Cluster->on_form_submit(ClassroomBot::onFormSubmit);
  m_Cluster->on_user_context_menu(ClassroomBot::onUserContext);
  m_Cluster->on_message_context_menu(ClassroomBot::onMessageContext);

  m_Cluster->on_ready([](const dpp::ready_t& event) {
    if (!dpp::run_once<struct register_bot_commands>()) return;
    if (m_Commands.size() == 0) return;

    std::vector<dpp::slashcommand> slashCommands;
    for (unsigned int i = 0; i < m_Commands.size(); i++) {
      for (dpp::slashcommand command : m_Commands[i]->spec().commands) {
        slashCommands.push_back(command.set_application_id(m_Cluster->me.id));
      }
    }

#ifdef DEBUG
    m_Cluster->guild_bulk_command_create(slashCommands, 705355899400880212); // feel free to join the support server :)
    m_Cluster->global_bulk_command_create(std::vector<dpp::slashcommand>()); // delete global commands in debug mode
#else
    m_Cluster->global_bulk_command_create(slashCommands);
#endif

    // register manage command to management server
    dpp::slashcommand manageCommand = m_ManageCommand->spec().commands[0];
    manageCommand.set_application_id(m_Cluster->me.id);
    m_Cluster->guild_command_create(manageCommand, MANAGEMENT_SERVER);

    updatePresence(0);
    cluster().start_timer(ClassroomBot::updatePresence, 60U);
  });

  m_Initialized = true;
}

bool ClassroomBot::run() {
  if (!m_Initialized) return m_Initialized;

  m_Cluster->start(false);

  return true;
}

void ClassroomBot::logMessage(const dpp::loglevel& ll, const std::string& message) {
  switch (ll) {
  case dpp::ll_trace:
    m_Logger->trace("{}", message);
    break;
  case dpp::ll_debug:
    m_Logger->debug("{}", message);
    break;
  case dpp::ll_info:
    m_Logger->info("{}", message);
    break;
  case dpp::ll_warning:
    m_Logger->warn("{}", message);
    break;
  case dpp::ll_error:
    m_Logger->error("{}", message);
    cluster().message_create(
        dpp::message("Error:\n" + message)
            .set_channel_id(ERROR_CRITICAL_CHANNEL)
            .set_guild_id(MANAGEMENT_SERVER)
    );
    break;
  case dpp::ll_critical:
  default:
    m_Logger->critical("{}", message);
    cluster().message_create(
        dpp::message("Critical:\n" + message)
            .set_channel_id(ERROR_CRITICAL_CHANNEL)
            .set_guild_id(MANAGEMENT_SERVER)
    );
    break;
  }
}

void ClassroomBot::log(const dpp::loglevel& ll, const std::string& message) {
  if (m_LogInitialized) logMessage(ll, message);
}

void ClassroomBot::onLog(const dpp::log_t& event) {
  if (m_LogInitialized) logMessage(event.severity, event.message);
}

void ClassroomBot::registerCommand(Command* command) {
  m_Commands.emplace_back(command);
}

void ClassroomBot::registerManageCommand(Command* command) {
  m_ManageCommand = command;
}

void ClassroomBot::onSlashCommand(const dpp::slashcommand_t& event) {
  for (Command* command : m_Commands) {
    const Command::CommandSpec& spec = command->spec();

    for (const dpp::slashcommand& slashCommand : spec.commands) {
      if (slashCommand.type != dpp::ctxm_chat_input) continue;

      if (slashCommand.name == event.command.get_command_name()) {
        LOG_DEBUG("Slash command " + slashCommand.name + " invoked by " + std::to_string(event.command.usr.id) + " in channel " + std::to_string(event.command.channel_id));
        command->command(CommandContext(event));
        return;
      }
    }
  }

  if (event.command.get_command_name() == "manage") {
    LOG_WARN("manage invoked by " + std::to_string(event.command.usr.id) + " in guild " + std::to_string(event.command.guild_id));
    m_ManageCommand->command(CommandContext(event));
    return;
  }
  LOG_WARN("Unknown command: \"" + event.command.get_command_name() + "\" issued by " + event.command.usr.username + " in " + std::to_string(event.command.guild_id));
}

void ClassroomBot::onSelectClick(const dpp::select_click_t& event) {
  for (Command* command : m_Commands) {
    const Command::CommandSpec& spec = command->spec();

    for (const std::string& menuId : spec.selectMenuIds) {
      if (menuId == event.custom_id) {
        LOG_DEBUG("Select menu " + menuId + " clicked by " + std::to_string(event.command.usr.id) + " in channel " + std::to_string(event.command.channel_id));
        command->selectClick(CommandContext(event));
        return;
      }
    }
  }
  LOG_WARN("Couldn't find handler for select click id: \"" + event.custom_id + "\" issued by " + event.command.usr.username + " in " + std::to_string(event.command.guild_id));
}

void ClassroomBot::onButtonClick(const dpp::button_click_t& event) {
  for (Command* command : m_Commands) {
    const Command::CommandSpec& spec = command->spec();

    for (const std::string& buttonId : spec.buttonIds) {
      if (buttonId == event.custom_id) {
        LOG_DEBUG("Button " + buttonId + " clicked by " + std::to_string(event.command.usr.id) + " in channel " + std::to_string(event.command.channel_id));
        command->buttonClick(CommandContext(event));
        return;
      }
    }
  }
  LOG_WARN("Couldn't find handler for button id: \"" + event.custom_id + "\" issued by " + event.command.usr.username + " in " + std::to_string(event.command.guild_id));
}

void ClassroomBot::onFormSubmit(const dpp::form_submit_t& event) {
  for (Command* command : m_Commands) {
    const Command::CommandSpec& spec = command->spec();

    for (const std::string modalId : spec.modalIds) {
      if (modalId == event.custom_id) {
        LOG_DEBUG("Form " + modalId + " submitted by " + std::to_string(event.command.usr.id) + " in channel " + std::to_string(event.command.channel_id));
        command->formSubmit(CommandContext(event));
        return;
      }
    }
  }
  LOG_WARN("Unknown form submit interaction: \"" + event.custom_id + "\" issued by " + event.command.usr.username + " in " + std::to_string(event.command.guild_id));
}

void ClassroomBot::onUserContext(const dpp::user_context_menu_t& event) {
  for (Command* command : m_Commands) {
    const Command::CommandSpec& spec = command->spec();

    for (const dpp::slashcommand& userContext : spec.commands) {
      if (userContext.type != dpp::ctxm_user) continue;

      if (userContext.name == event.command.get_command_name()) {
        LOG_DEBUG("User context item " + userContext.name + " used by " + std::to_string(event.command.usr.id) + " in channel " + std::to_string(event.command.channel_id));
        command->userContext(CommandContext(event));
        return;
      }
    }
  }
  LOG_WARN("Unknown user context command: \"" + event.command.get_command_name() + "\" issued by " + event.command.usr.username + " in " + std::to_string(event.command.guild_id));
}

void ClassroomBot::onMessageContext(const dpp::message_context_menu_t& event) {
  // TODO
}

void ClassroomBot::updatePresence(dpp::timer timer) {
  LOG_INFO("Uptime: " + std::to_string(m_Cluster->uptime().hours) + " hours");

  unsigned int guildCount = 0;
  for (auto shard : m_Cluster->get_shards()) {
    guildCount += shard.second->get_guild_count();
  }

  switch (m_PresenceState) {
  case 0:
    m_Cluster->set_presence(dpp::presence(dpp::presence_status::ps_online, dpp::activity_type::at_listening, "/"));
    break;

  case 1:
    m_Cluster->set_presence(dpp::presence(dpp::presence_status::ps_online, dpp::activity_type::at_custom, std::to_string(guildCount) + " servers"));
    break;

  case 2:
    m_Cluster->set_presence(dpp::presence(dpp::presence_status::ps_online, dpp::activity_type::at_game, "V" + std::string(BOT_VERSION)));
    break;

  default:
    break;
  }

  m_PresenceState++;
  m_PresenceState = m_PresenceState % 3;

  HandRepo::expire();
  QuestionRepo::expire();

  if (guildCount == 0) return;

  if (Config::get()["topgg"]["enable"] == "true") {
    std::string topggToken = Config::get()["topgg"]["token"];
    std::string topggId    = Config::get()["topgg"]["bot_id"];

    Poco::Net::HTTPSClientSession session("top.gg", 443);
    Poco::Net::HTTPRequest        req("POST", "/api/bots/" + topggId + "/stats");

    req.setCredentials("", topggToken);
    req.setContentType("application/json");

    std::string body = "{\"server_count\": " + std::to_string(guildCount) + "}";
    req.setContentLength(body.length());

    session.sendRequest(req) << body;

    Poco::Net::HTTPResponse response;
    session.receiveResponse(response);
  }
}

dpp::cluster& ClassroomBot::cluster() {
  return *m_Cluster;
}