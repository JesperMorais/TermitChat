#include "enter_name_menu.hpp"
#include "menu_state.hpp"
#include "handle_uuid.hpp"

string client_name;
string input_content_client_name;

int rund_app() {
    AppState app_state = AppState::EnterClientName;
    auto screen = ftxui::ScreenInteractive::Fullscreen();

    //Skapar enter_name_menu som håller den skärmen SAMT skapar vi lambda för vad som ska ske vid knapp tryckning.
    auto enter_name_menu = MakeEnterNameMenu(&input_content_client_name, &client_name, [&] {
        app_state = AppState::ServerSelect;
        screen.PostEvent(ftxui::Event::Custom);
    });

    vector<string> servers = {"Server A", "Server B", "Server C"};

    //Skapar server_overViewMenu som håller den skärmen SAMT skapar vi lambda för vad som ska ske vid knapp tryckning.
    auto server_overview_menu = MakeServerOverview(servers, [&](const std::string& server_name){
        
        app_state = AppState::Exit;
        screen.PostEvent(ftxui::Event::Custom); //säger till att en uppdaterign skett vilket kommer köra renderingen igen
    });


    //har hand om olika komponenter i renderingen, Lägg till nya menyer här
    int current_tab = 0;
    auto tab_container = Container::Tab(
        {
            enter_name_menu, // 0
            server_overview_menu // 1
        },
        &current_tab
    );

    //Main renderer, håller koll på vilken STATE vi är i samt ändrar TAB beroende på detta. Detta kommer i sin tur aktivera "tab container" som retunerar rätt komponent
    auto main_renderer = Renderer(tab_container, [&] {
        switch (app_state) {
            case AppState::EnterClientName: 
                current_tab = 0;
                break;
            case AppState::ServerSelect:
                current_tab = 1;
                break;
            default:
                return text("Invalid state!") | color(Color::Red);
        }
        return tab_container->Render();
    });

    screen.Loop(main_renderer);

    return 0;
}
