#include "attendanceCommand.h"

#include <dpp/fmt/format.h>

#include <bot/bot.h>
#include <bot/util/util.h>

const std::string buttonId = "attend";

AttendanceCommand::AttendanceCommand() {
  {
    auto& command = m_Spec.commands.emplace_back();

    command.set_name("attendance")
    .set_description("Take attendance.")
    .set_dm_permission(false);
  }

  m_Spec.buttonIds.emplace_back(buttonId);
}

const dpp::component presentButton() {
  return dpp::component().add_component(
    dpp::component().set_label("I'm present")
    .set_type(dpp::cot_button)
    .set_emoji("🖐️")
    .set_style(dpp::cos_primary)
    .set_id(buttonId)
  );
}

void AttendanceCommand::command(const CommandContext& ctx) {
  ctx.confirm();
  ctx.replyUnlocalizedChannel(dpp::message()
    .set_content(fmt::format("{teacher} is taking attendance", fmt::arg("teacher", mentionUser(ctx.userId()))))
    .add_component(
      presentButton()
    )
  );
}

void AttendanceCommand::userContext(const CommandContext& ctx) {
}

void AttendanceCommand::messageContext(const CommandContext& ctx) {
}

void AttendanceCommand::buttonClick(const CommandContext& ctx) {
  // TODO: potentially unsafe
  dpp::button_click_t& event = (dpp::button_click_t&)ctx.event;

  const dpp::snowflake& userId = event.command.member.user_id;
  std::string messageContents = event.command.msg.content;

  if(messageContents.find('\n') == std::string::npos) {
    messageContents += "\n\nPeople present:";
  }

  std::string userIdString = std::to_string(userId);

  if(messageContents.find(userIdString, messageContents.find_first_of('\n')) == std::string::npos) {
    messageContents += "\n" + mentionUser(userId);
    ctx.confirm();
  } else {
    ctx.replyUnlocalized(dpp::message("You have already indicated attendance").set_flags(dpp::m_ephemeral));
  }

  dpp::message message = dpp::message(ctx.channelId(), messageContents).add_component(presentButton());
  message.id = event.command.message_id;

  ClassroomBot::cluster().message_edit(message);
}

void AttendanceCommand::selectClick(const CommandContext& ctx) {

}

void AttendanceCommand::formSubmit(const CommandContext& ctx) {

}
