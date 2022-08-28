#pragma once

#include <bot/persistence/model/settings.h>
#include <dpp/dpp.h>

#include <string>
#include <vector>

class CommandContext {
public:
  enum EventType {
    AutoComplete,
    ButtonClick,
    MessageContext,
    UserContext,
    FormSubmit,
    SelectClick,
    SlashCommand
  };

  template <class T>
  CommandContext(const T& event) :
    event(event),
    eventType(eventTypeFromType<T>()),
    m_UserLangCode(CommandContext::event.command.locale),
    m_GuildLangCode(CommandContext::event.command.guild_locale) {
  }

  const dpp::snowflake& userId() const;
  const dpp::snowflake& channelId() const;
  const dpp::snowflake& guildId() const;

  // emote replies
  void confirm() const;
  void deny() const;
  void wait() const;
  void mute() const;
  void unmute() const;

  // acknowledge command
  void reply() const;

  // respond with a modal dialog
  void dialog(const dpp::interaction_modal_response& mr) const;

  // string replies
  void               replyUnlocalized(const std::string& message, bool ephemeral = false) const;
  void               replyUnlocalized(const dpp::message& message) const;
  void               replyLocalized(const std::string& name, bool ephemeral = false) const;
  void               replyUnlocalizedChannel(dpp::message message) const;
  const std::string& localize(const std::string& name) const;
  const std::string& localizeGuild(const std::string& name) const;

  const std::string& userLanguage() const;

  // specific getters
  dpp::autocomplete_t&         autocomplete() const;
  dpp::button_click_t&         buttonClick() const;
  dpp::message_context_menu_t& messageContext() const;
  dpp::user_context_menu_t&    userContext() const;
  dpp::form_submit_t&          formSubmit() const;
  dpp::select_click_t&         selectClick() const;
  dpp::slashcommand_t          slashCommand() const;

  const EventType                  eventType;
  const dpp::interaction_create_t& event; // TODO: Make this a private member

private:
  const std::string& m_UserLangCode;
  const std::string& m_GuildLangCode;

#define ReturnEventIfSameTypes(type1, type2, value) \
  if constexpr (std::is_same_v<type1, type2>) {     \
    return value;                                   \
  }

  template <class Type>
  constexpr EventType eventTypeFromType() {
    ReturnEventIfSameTypes(Type, dpp::autocomplete_t, AutoComplete);
    ReturnEventIfSameTypes(Type, dpp::button_click_t, ButtonClick);
    ReturnEventIfSameTypes(Type, dpp::message_context_menu_t, MessageContext);
    ReturnEventIfSameTypes(Type, dpp::user_context_menu_t, UserContext);
    ReturnEventIfSameTypes(Type, dpp::form_submit_t, FormSubmit);
    ReturnEventIfSameTypes(Type, dpp::select_click_t, SelectClick);
    ReturnEventIfSameTypes(Type, dpp::slashcommand_t, SlashCommand);
  }
};

class Command {
public:
  struct CommandSpec {
    std::vector<dpp::slashcommand> commands;      // slash commands from this command (a command may have multiple). This includes context menu interactions.
    std::vector<std::string>       buttonIds;     // custom button id's handeled by this command
    std::vector<std::string>       selectMenuIds; // select menu id's handeled by this command
    std::vector<std::string>       modalIds;      // modal id's handeled by this command
  };

  // handlers for different interactions
  virtual void command(const CommandContext& ctx);        // called when a slash command is issued
  virtual void userContext(const CommandContext& ctx);    // called when a user context command is issued
  virtual void messageContext(const CommandContext& ctx); // called when a message context command is issued
  virtual void buttonClick(const CommandContext& ctx);    // called when a button is pressed
  virtual void selectClick(const CommandContext& ctx);    // called when a select menu item is clicked
  virtual void formSubmit(const CommandContext& ctx);     // called when a form is submitted

  // return the command spec
  const CommandSpec& spec();

protected:
  CommandSpec m_Spec; // specification for this command, should be filled out
                      // in the constructor. spec() returns a reference to this
                      // when command is registered
};

// we do a little function overloading :)
// add localization to a slash command or command option if the language has
// defined the name and description strings, useful when registering commands
void addLocalizationIfExists(
    dpp::slashcommand& command,
    const std::string& language,
    const std::string& name,
    const std::string& description
);
void addLocalizationIfExists(
    dpp::command_option& command,
    const std::string&   language,
    const std::string&   name,
    const std::string&   description
);