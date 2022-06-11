#include "attendanceCommand.h"

#include <dpp/fmt/format.h>

#include <bot/bot.h>
#include <bot/util/util.h>
#include <bot/localization/localization.h>

const std::string buttonId = "attend";

AttendanceCommand::AttendanceCommand() {
  {
    auto& command = m_Spec.commands.emplace_back(Localization::getString("attendance_cmd_attendance"), Localization::getString("attendance_cmd_attendance_desc"), NULL);

    for(const auto& lang : Localization::getLanguages()) {
      const std::string& currentLangCode = lang.first;

      if(currentLangCode == "en-US") continue;

      addLocalizationIfExists(command, currentLangCode, "attendance_cmd_attendance", "attendance_cmd_attendance_desc");
    }
  }

  m_Spec.buttonIds.emplace_back(buttonId);
}

const dpp::component presentButton(const CommandContext& ctx) {
  return dpp::component().add_component(
    dpp::component().set_label(ctx.localizeGuild("attendance_present_button"))
    .set_type(dpp::cot_button)
    .set_emoji("🖐️")
    .set_style(dpp::cos_primary)
    .set_id(buttonId)
  );
}

void AttendanceCommand::command(const CommandContext& ctx) {
  ctx.confirm();
  ctx.replyUnlocalizedChannel(dpp::message()
    .set_content(fmt::format(ctx.localizeGuild("attendance_taking_attendance"), fmt::arg("teacher", mentionUser(ctx.userId()))))
    .add_component(presentButton(ctx))
  );
}

void AttendanceCommand::buttonClick(const CommandContext& ctx) {
  // TODO: potentially unsafe cast
  dpp::button_click_t& event = (dpp::button_click_t&)ctx.event;

  const dpp::snowflake& userId = event.command.member.user_id;
  std::string messageContents = event.command.msg.content;

  if(messageContents.find('\n') == std::string::npos) {
    messageContents += "\n\n" + ctx.localizeGuild("attendance_people_present") + ":";
  }

  std::string userIdString = std::to_string(userId);

  if(messageContents.find(userIdString, messageContents.find_first_of('\n')) == std::string::npos) {
    messageContents += "\n" + mentionUser(userId);
    ctx.reply();
  } else {
    ctx.replyLocalized("attendance_already_indicated", true);
  }

  event.command.msg.set_content(messageContents);

  ClassroomBot::cluster().message_edit(event.command.msg);
}
