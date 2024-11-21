#include <ftxui/component/screen_interactive.hpp>
#include <ftxui/component/component.hpp>
#include <ftxui/dom/elements.hpp>
#include "ftxui/dom/node.hpp"      // for Render
#include "ftxui/screen/color.hpp"  // for ftxui

#include <string>
#include <vector>

using namespace ftxui;
using namespace std;

auto main() -> int
{
  using namespace ftxui;
  auto screen = ScreenInteractive::TerminalOutput();

  string username;
  string password;

  Component input_user_name = Input(&username, "First name");

  InputOption password_option;
  password_option.password = true;
  Component input_password = Input(&password, "password", password_option);

  auto render2 = Renderer([&]
                          { return vbox({
                                hbox(text(" TEST")),
                            }); });

  auto on_confim = [&]
  {
    if (password.empty() == false)
    {
      screen.Loop(render2);
    }
  };

  Component confirm_button = Button("Confirm", on_confim);

  auto component = Container::Vertical({
      input_user_name,
      input_password,
      confirm_button,
  });

  auto render = Renderer(component, [&]
                         { return vbox({
                                      hbox(text(" First name : "), input_user_name->Render()),
                                      hbox(text(" Password : "), input_password->Render()),
                                      confirm_button->Render(),
                                      separator(),
                                      text("hello " + username + ", your password is: " + password),
                                  }) |
                                  border; });

  screen.Loop(render);
  return 0;
}
