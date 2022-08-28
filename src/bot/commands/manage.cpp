#include <bot/bot.h>
#include <bot/commands/manage.h>
#include <bot/tasks/migratedb.h>
#include <fmt/format.h>

ManageCommand::ManageCommand() :
  Command::Command() {
  // application ids are overwritten by main class when registering commands, set them to NULL for now
  dpp::slashcommand& manageCommand = m_Spec.commands.emplace_back("manage", "Various commands to manage the bot. Can only be run in in the management server by admins", NULL);

  dpp::command_option optionTask(dpp::co_sub_command, "task", "Run a task");
  dpp::command_option optionTaskOptionName(dpp::co_string, "name", "Name of the task to run", true);

  optionTask.add_option(optionTaskOptionName);

  manageCommand.add_option(optionTask);
}

// Discord event handlers

void ManageCommand::command(const CommandContext& ctx) {
  dpp::command_interaction ci = ctx.slashCommand().command.get_command_interaction();

  if (ctx.guildId() != MANAGEMENT_SERVER) {
    LOG_CRITICAL(
        fmt::format(
            "!!!!!! manage invoked outside of management server !!!!!!\n"
            "Invoked by user {invoker} in guild {guild}",
            fmt::arg("invoker", std::to_string(ctx.userId())),
            fmt::arg("guild", std::to_string(ctx.guildId()))
        )
    );
  }

  const std::string& commandName = ci.options[0].name;

  if (commandName == "task")
    taskCmd(ctx);
  else {
    LOG_WARN("Manage: unknown command: " + commandName);
    ctx.replyUnlocalized("Unknown command", true);
  }
}

// Command handlers

void ManageCommand::taskCmd(const CommandContext& ctx) {
  dpp::command_interaction ci = ctx.event.command.get_command_interaction();

  if (ci.options.size() == 0) {
    ctx.replyUnlocalized("Please enter a task name");
    return;
  }

  const std::string& taskName = std::get<std::string>(ci.options[0].options[0].value);

  if (taskName == "migratedb")
    taskMigratedb(ctx);
  else {
    LOG_WARN("ManageCommand: unknown task: " + taskName);
    ctx.replyUnlocalized("Unknown task " + taskName);
  }
}
