#include "enter_name_menu.hpp"
#include "menu_state.hpp"
#include "handle_uuid.hpp"

AppState app_state;
string client_name;
string input_content_client_name;

int rund_app() {
    AppState app_state = AppState::EnterClientName;
    auto screen = ScreenInteractive::TerminalOutput();

    std::string client_name;
    std::string input_content_client_name;

    std::vector<std::string> servers = { "Server A", "Server B", "Server C" };

    // Skapa de olika vy-komponenterna först:
    auto enter_name_menu = MakeEnterNameMenu(&input_content_client_name, &client_name, [&] {
        // När on_submit anropas byter vi state.
        app_state = AppState::ServerOW;
        // Screen kommer uppdateras automatiskt. 
        // Ingen ExitLoopClosure() behövs här om du vill fortsätta i samma loop.
        // Om du vill avsluta loopen använder du screen.PostEvent(ftxui::Event::Escape) eller liknande.
    });

    

    auto main_renderer = Renderer([&] {
        switch (app_state) {
            case AppState::EnterClientName: 
                return enter_name_menu->Render();
            default:
                return text("Invalid state!") | color(Color::Red);
        }
    });

    screen.Loop(main_renderer);

    return 0;
}
