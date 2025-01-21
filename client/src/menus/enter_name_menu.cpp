#include "enter_name_menu.hpp"
#include <ftxui/component/component.hpp>
#include <ftxui/component/screen_interactive.hpp> 
#include <ftxui/dom/elements.hpp>
using namespace ftxui;

ftxui::Component MakeEnterNameMenu(string* input_content_client_name, string* client_name, function<void()> on_submit) {
    
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

   // Renderer för att skapa utseendet
    auto renderer = Renderer(container, [=] {
        return vbox({
            text("Välkommen till Termit!") | bold | center | color(Color::Blue),
            separator(),
            text("Ange client namn för att fortsätta:"),
            separator(),
            client_input->Render() | border,
            separator(),
            client_button->Render(),
            separator()
        });
    });
    return renderer;
}


Component MakeServerOverview(const vector<string>& servers, function<void(const string&)> on_connect) {
    // Radiobox för att välja server
    int selected_server = 1;
    auto server_list = Radiobox({&servers, selected_server});

    // Knapp för att ansluta till vald server
    auto connect_button = Button("Anslut", [=] {
        // Anropa callbacken med namnet på den valda servern
        
        on_connect(servers[selected_server]);
    });

    // Lägg ihop komponenterna i ett container
    auto container = Container::Vertical({
        server_list,
        connect_button,
    });

    // Renderer för UI
    auto renderer = Renderer(container, [=] {
        return vbox({
            text("Tillgängliga servrar:") | bold | center,
            separator(),
            server_list->Render() | border | flex | color(Color::Green3Bis),
            separator(),
            connect_button->Render(),
        }) | border | center;
    });

    return renderer;
}