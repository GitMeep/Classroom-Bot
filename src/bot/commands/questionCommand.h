#pragma once

#include <bot/commands/command.h>

// /ask

// /drop

// /questions (list questions)

// /answer next (answer next question)
// /answer user @user (answer specific user)
// /answer list (show pick menu)
// /answer random (answer random question)
// /answer clear (clear questions)

class QuestionCommand : public Command {
public:
  QuestionCommand();

  void command(const CommandContext& ctx);
  void selectClick(const CommandContext& ctx);
  void formSubmit(const CommandContext& ctx);

private:
  // slash commands
  void askCmd(const CommandContext& ctx);

  void dropCmd(const CommandContext& ctx);

  void questionsCmd(const CommandContext& ctx);

  void answerCmd(const CommandContext& ctx);
  // /pick subcommands
  void answerNext(const CommandContext& ctx);
  void answerUser(const CommandContext& ctx);
  void answerList(const CommandContext& ctx);
  void answerRandom(const CommandContext& ctx);
  void answerClear(const CommandContext& ctx);

  // modal
  void askFromModal(const CommandContext& ctx);
  void answerFromModal(const CommandContext& ctx);

  // select menu
  void dropFromList(const CommandContext& ctx);
  void answerFromList(const CommandContext& ctx);
};