#include "enter_name_menu.hpp"
#include <ftxui/component/component.hpp>
#include <ftxui/component/screen_interactive.hpp> 
#include <ftxui/dom/elements.hpp>
using namespace ftxui;

ftxui::Component MakeEnterNameMenu(std::string* input_content_client_name, std::string* client_name, std::function<void()> on_submit) {
    auto client_input = Input(input_content_client_name, "skriv namn...");
    auto client_button = Button("Fortsätt..", [=] {
        if(!input_content_client_name->empty()) {
            *client_name = *input_content_client_name;
            on_submit();
        } else {
            std::cout << "no empty username allowed" << std::endl;
        }
    });

    auto container = Container::Vertical({
        client_input,
        client_button,
    });

    auto renderer = Renderer(container, [=] {
        return vbox({
            text("Välkommen till Termit!") | bold | center | color(Color::Blue),
            separator(),
            text("Ange client namn för att fortsätta:"),
            client_input->Render() | border,
            client_button->Render(),
            separator()
        });
    });
    return renderer;
}