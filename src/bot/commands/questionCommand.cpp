#include <bot/bot.h>
#include <bot/commands/questionCommand.h>
#include <bot/localization/localization.h>
#include <bot/persistence/repo/questionRepo.h>
#include <bot/util/util.h>
#include <fmt/format.h>

const std::string questionAnswerMenuId = "question_menu_answer_user";
const std::string questionDropMenuId   = "question_menu_drop";

const std::string questionAskModalId = "question_modal_ask";

QuestionCommand::QuestionCommand() :
  Command::Command() {
  {
    // TODO: On all commands: startup performance isn't that critical, we don't need to construct the commands in place in the vector, just construct them in the local scope and push_back them.
    m_Spec.commands.reserve(4); // Set to number of commands to prevent the vector from moving the data and invalidating the references

    // application ids are overwritten by main class when registering commands, set them to NULL for now
    dpp::slashcommand&  askCommand = m_Spec.commands.emplace_back(Localization::getString("question_cmd_ask"), Localization::getString("question_cmd_ask_desc"), NULL);
    dpp::command_option optionAskOptionQuestion(dpp::co_string, Localization::getString("question_cmd_ask_option_question"), Localization::getString("question_cmd_ask_option_question_desc"));

    dpp::slashcommand& dropCommand = m_Spec.commands.emplace_back(Localization::getString("question_cmd_drop"), Localization::getString("question_cmd_drop_desc"), NULL);

    dpp::slashcommand& questionsCommand = m_Spec.commands.emplace_back(Localization::getString("question_cmd_questions"), Localization::getString("question_cmd_questions_desc"), NULL);

    dpp::slashcommand& answerCommand = m_Spec.commands.emplace_back(Localization::getString("question_cmd_answer"), Localization::getString("question_cmd_answer_desc"), NULL)
                                           .set_default_permissions(dpp::p_manage_messages);
    dpp::command_option optionAnswerNext(dpp::co_sub_command, Localization::getString("question_cmd_answer_option_next"), Localization::getString("question_cmd_answer_option_next_desc"));
    dpp::command_option optionAnswerUser(dpp::co_sub_command, Localization::getString("question_cmd_answer_option_user"), Localization::getString("question_cmd_answer_option_user_desc"));
    dpp::command_option optionAnswerUserOptionUser(dpp::co_user, Localization::getString("question_cmd_answer_option_user_option_user"), Localization::getString("question_cmd_answer_option_user_option_user_desc"));
    dpp::command_option optionAnswerList(dpp::co_sub_command, Localization::getString("question_cmd_answer_option_list"), Localization::getString("question_cmd_answer_option_list_desc"));
    dpp::command_option optionAnswerRandom(dpp::co_sub_command, Localization::getString("question_cmd_answer_option_random"), Localization::getString("question_cmd_answer_option_random_desc"));
    dpp::command_option optionAnswerClear(dpp::co_sub_command, Localization::getString("question_cmd_answer_option_clear"), Localization::getString("question_cmd_answer_option_clear_desc"));

    for (const auto& lang : Localization::getLanguages()) {
      const std::string& currentLangCode = lang.first;

      if (currentLangCode == "en-US") continue;

      addLocalizationIfExists(askCommand, currentLangCode, "question_cmd_ask", "question_cmd_ask_desc");
      addLocalizationIfExists(optionAskOptionQuestion, currentLangCode, "question_cmd_ask_option_question", "question_cmd_ask_option_question_desc");

      addLocalizationIfExists(dropCommand, currentLangCode, "question_cmd_drop", "question_cmd_drop_desc");

      addLocalizationIfExists(questionsCommand, currentLangCode, "question_cmd_questions", "question_cmd_questions_desc");

      addLocalizationIfExists(answerCommand, currentLangCode, "question_cmd_answer", "question_cmd_answer_desc");
      addLocalizationIfExists(optionAnswerNext, currentLangCode, "question_cmd_answer_option_next", "question_cmd_answer_option_next_desc");
      addLocalizationIfExists(optionAnswerList, currentLangCode, "question_cmd_answer_option_list", "question_cmd_answer_option_list_desc");
      addLocalizationIfExists(optionAnswerUser, currentLangCode, "question_cmd_answer_option_user", "question_cmd_answer_option_user_desc");
      addLocalizationIfExists(optionAnswerUserOptionUser, currentLangCode, "question_cmd_answer_option_user", "question_cmd_answer_option_user_desc");
      addLocalizationIfExists(optionAnswerRandom, currentLangCode, "question_cmd_answer_option_random", "question_cmd_answer_option_random_desc");
    }

    askCommand.add_option(optionAskOptionQuestion);

    answerCommand
        .add_option(optionAnswerNext)
        .add_option(optionAnswerUser
                        .add_option(optionAnswerUserOptionUser))
        .add_option(optionAnswerList)
        .add_option(optionAnswerRandom)
        .add_option(optionAnswerClear);
  }

  // hand pick menu
  m_Spec.selectMenuIds.emplace_back(questionAnswerMenuId);
  m_Spec.selectMenuIds.emplace_back(questionDropMenuId);

  m_Spec.modalIds.emplace_back(questionAskModalId);
}

// Discord event handlers

void QuestionCommand::command(const CommandContext& ctx) {
  dpp::command_interaction ci = ctx.slashCommand().command.get_command_interaction();

  const std::string& commandName = ci.name;

  if (commandName == Localization::getString("question_cmd_ask"))
    askCmd(ctx);
  else if (commandName == Localization::getString("question_cmd_drop"))
    dropCmd(ctx);
  else if (commandName == Localization::getString("question_cmd_questions"))
    questionsCmd(ctx);
  else if (commandName == Localization::getString("question_cmd_answer"))
    answerCmd(ctx);
  else {
    LOG_WARN("Question: unknown command: " + commandName);
    ctx.replyUnlocalized("Unknown command", true);
  }
}

void QuestionCommand::selectClick(const CommandContext& ctx) {
  dpp::select_click_t& event = ctx.selectClick();

  const std::string& menuId = event.custom_id;

  if (menuId == questionAnswerMenuId)
    answerFromList(ctx);
  else if (menuId == questionDropMenuId)
    dropFromList(ctx);
  else {
    LOG_WARN("QuestionCommand: unknown select menu id: " + menuId);
    ctx.replyUnlocalized("Unknown select menu");
  }
}

void QuestionCommand::formSubmit(const CommandContext& ctx) {
  dpp::form_submit_t& event = ctx.formSubmit();

  const std::string& modalId = event.custom_id;

  if (modalId == questionAskModalId)
    askFromModal(ctx);
  else {
    LOG_WARN("QuestionCommand: unknown modal id: " + modalId);
    ctx.replyUnlocalized("Unknown modal");
  }
}

// Helper functions

dpp::message createAnswerListMenu(const CommandContext& ctx, const std::deque<Question>& questions, const dpp::snowflake& guildId) {
  dpp::component listComponent = dpp::component()
                                     .set_type(dpp::cot_selectmenu)
                                     .set_placeholder(ctx.localize("question_menu_answer_placeholder"))
                                     .set_id(questionAnswerMenuId);

  for (auto& question : questions) {
    listComponent.add_select_option(dpp::select_option(question.question.substr(0, 100), question.questionId, nickOrUsername(question.userId, guildId)));
  }

  return dpp::message().add_component(dpp::component().add_component(listComponent)).set_flags(dpp::m_ephemeral);
}

dpp::message createDropQuestionMenu(const CommandContext& ctx, const std::deque<Question>& questions, const dpp::snowflake& guildId) {
  dpp::component listComponent = dpp::component()
                                     .set_type(dpp::cot_selectmenu)
                                     .set_placeholder(ctx.localize("question_menu_drop_placeholder"))
                                     .set_id(questionDropMenuId);

  for (auto& question : questions) {
    listComponent.add_select_option(dpp::select_option(question.question.substr(0, 100), question.questionId));
  }

  return dpp::message().add_component(dpp::component().add_component(listComponent)).set_flags(dpp::m_ephemeral);
}

dpp::interaction_modal_response createQuestionModal(const CommandContext& ctx) {
  return dpp::interaction_modal_response(
      questionAskModalId,
      ctx.localize("question_modal_ask_title"),
      {dpp::component()
           .set_id("unused")
           .set_label(ctx.localize("question_modal_ask_label"))
           .set_type(dpp::cot_text)
           .set_placeholder(ctx.localize("question_modal_ask_placeholder"))
           .set_min_length(1)
           .set_max_length(560)
           .set_text_style(dpp::text_paragraph)}
  );
}

// register question if queue isn't full and question isn't empty
void registerQuestionAndConfirm(const CommandContext& ctx, const std::string& question) {
  auto questions = QuestionRepo::getByChannel(ctx.channelId());

  if (questions.size() >= 50) {
    ctx.replyLocalized("question_limit", true);
    return;
  }

  if (question == "") {
    ctx.replyLocalized("empty_question", true);
    return;
  }

  QuestionRepo::ask(ctx.channelId(), ctx.userId(), question);
  ctx.confirm();
}

void replyAnswerQuestion(const CommandContext& ctx, const Question& question) {
  dpp::message message(
      fmt::format(
          ctx.localizeGuild("question_answer_answering_message"),
          fmt::arg("answerer", mentionUser(ctx.userId())),
          fmt::arg("asker", mentionUser(question.userId))
      ) +
      "```" + question.question + "```"
  );
  message.allowed_mentions.users.push_back(question.userId);
  ctx.replyUnlocalized(message);
}

// Command handlers

void QuestionCommand::questionsCmd(const CommandContext& ctx) {
  auto questions = QuestionRepo::getByChannel(ctx.channelId());

  if (!questions.size()) {
    ctx.replyLocalized("no_questions", true);
    return;
  }

  std::stringstream ss;

  auto it = questions.cbegin();
  while (it != questions.cend()) {
    ss << mentionUser(it->userId) << ": ```" << it->question << "```" << std::endl;
    it++;
  }

  ctx.replyUnlocalized(ss.str(), true);
}

void QuestionCommand::askCmd(const CommandContext& ctx) {
  dpp::command_interaction ci = ctx.slashCommand().command.get_command_interaction();

  if (ci.options.size() == 0) {
    ctx.dialog(createQuestionModal(ctx));
    return;
  }

  std::string question = std::get<std::string>(ci.options[0].value);

  registerQuestionAndConfirm(ctx, question);
}

void QuestionCommand::dropCmd(const CommandContext& ctx) {
  std::deque<Question> questions = QuestionRepo::getByChannelAndUser(ctx.channelId(), ctx.userId());

  if (questions.size() <= 1) {
    if (questions.size() == 1) {
      QuestionRepo::dismiss(questions[0].questionId);
    }
    ctx.confirm();
    return;
  }

  ctx.replyUnlocalized(
      createDropQuestionMenu(ctx, questions, ctx.guildId())
  );
}

void QuestionCommand::answerCmd(const CommandContext& ctx) {
  dpp::command_interaction ci = ctx.event.command.get_command_interaction();

  if (ci.options.size() == 0) {
    ctx.replyLocalized("something_went_wrong");
    return;
  }

  const std::string& subCommand = ci.options[0].name;

  if (subCommand == Localization::getString("question_cmd_answer_option_next"))
    answerNext(ctx);
  else if (subCommand == Localization::getString("question_cmd_answer_option_list"))
    answerList(ctx);
  else if (subCommand == Localization::getString("question_cmd_answer_option_user"))
    answerUser(ctx);
  else if (subCommand == Localization::getString("question_cmd_answer_option_random"))
    answerRandom(ctx);
  else if (subCommand == Localization::getString("question_cmd_answer_option_clear"))
    answerClear(ctx);
  else {
    LOG_WARN("QuestionCommand: unknown subcommand: " + subCommand);
    ctx.replyUnlocalized("Unknown subcommand");
  }
}

// answer subcommands

void QuestionCommand::answerNext(const CommandContext& ctx) {
  Question question = QuestionRepo::getNextByChannel(ctx.channelId());

  if (question.questionId == "") {
    ctx.replyLocalized("no_questions", true);
    return;
  }

  QuestionRepo::dismiss(question.questionId);

  replyAnswerQuestion(ctx, question);
}

void QuestionCommand::answerUser(const CommandContext& ctx) {
  dpp::command_interaction ci = ctx.slashCommand().command.get_command_interaction();

  if (ci.options[0].options.size() == 0) {
    ctx.replyLocalized("question_cmd_answer_option_user_no_user");
    return;
  }

  if (ci.options[0].options[0].type != dpp::co_user) {
    ctx.replyUnlocalized("Expected a user as first argument to pick");
    return;
  }

  const dpp::snowflake& userId = std::get<dpp::snowflake>(ci.options[0].options[0].value);

  std::deque<Question> questions = QuestionRepo::getByChannelAndUser(ctx.channelId(), userId);

  if (questions.size() == 0) {
    ctx.replyLocalized("user_no_questions", true);
    return;
  }

  ctx.replyUnlocalized(
      createAnswerListMenu(ctx, questions, ctx.guildId()).set_content(ctx.localize("question_answer_howto"))
  );
}

void QuestionCommand::answerList(const CommandContext& ctx) {
  std::deque<Question> questions = QuestionRepo::getByChannel(ctx.channelId());

  if (questions.size() == 0) {
    ctx.replyLocalized("no_questions", true);
    return;
  }

  ctx.replyUnlocalized(
      createAnswerListMenu(ctx, questions, ctx.guildId()).set_content(ctx.localize("question_answer_howto"))
  );
}

void QuestionCommand::answerRandom(const CommandContext& ctx) {
  Question question = QuestionRepo::getRandomByChannel(ctx.channelId());

  if (question.questionId == "") {
    ctx.replyLocalized("no_questions", true);
    return;
  }

  QuestionRepo::dismiss(question.questionId);

  replyAnswerQuestion(ctx, question);
}

void QuestionCommand::answerClear(const CommandContext& ctx) {
  QuestionRepo::clearChannel(ctx.channelId());

  ctx.confirm();
}

// modal handlers

void QuestionCommand::askFromModal(const CommandContext& ctx) {
  std::string question = std::get<std::string>(ctx.formSubmit().components[0].components[0].value);

  registerQuestionAndConfirm(ctx, question);
}

// Select menu click handler(s)

void QuestionCommand::dropFromList(const CommandContext& ctx) {
  std::string questionId = ctx.selectClick().values[0];

  QuestionRepo::dismiss(questionId);

  ctx.confirm();
}

void QuestionCommand::answerFromList(const CommandContext& ctx) {
  const std::string& questionId = ctx.selectClick().values[0];

  Question question = QuestionRepo::getAndDismiss(questionId);

  if (question.questionId == "") {
    ctx.replyLocalized("question_answer_already_answered", true);
    return;
  }

  replyAnswerQuestion(ctx, question);
}
