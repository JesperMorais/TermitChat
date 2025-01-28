#include "enter_name_menu.hpp"
#include "menu_state.hpp"
#include "handle_uuid.hpp"
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/uuid_io.hpp>
#include "global_params.hpp"

using namespace boost::uuids;
string client_name;
string input_content_client_name;

//Funktion som hämtar serverar som vi kan connecta till


int rund_app(void* params) {
    thread_params* params_d = (thread_params*)params;
    
    AppState app_state = AppState::EnterClientName;
    auto screen = ftxui::ScreenInteractive::Fullscreen();

    //Skapar enter_name_menu som håller den skärmen SAMT skapar vi lambda för vad som ska ske vid knapp tryckning.
    auto enter_name_menu = MakeEnterNameMenu(&input_content_client_name, &client_name, [&] {
        
        //Kanske skapa en funktion för detta------
        uuid id = random_generator()();
        stringstream ss;
        ss << id;
        string uuid_str = ss.str();
        //----------------------------------------
        if(client_name.empty())
            params_d->clientInput_username = "NIL"; 
        else    
            params_d->clientInput_username = client_name;

        //sätter till true sen notifyar mqtt tasken att börja
        {
            std::lock_guard<std::mutex> lock(params_d->m);
            params_d->usernameSet = true;
            params_d->mqttClient_ID = uuid_str;
        }
        params_d->cv.notify_one();

        app_state = AppState::ServerSelect;
        screen.PostEvent(ftxui::Event::Custom);
    });

    string server_name_;
    //Skapar server_overViewMenu som håller den skärmen SAMT skapar vi lambda för vad som ska ske vid knapp tryckning.
    auto server_overview_menu = MakeServerOverview([&](const std::string& server_name){
        
        logfile << "Trying to connect to server: " << server_name << std::endl;
        //Ändra app_state till connected to serverX 
        server_name_ = server_name;
        params_d->current_server_name = server_name; // sätter current_server_name till en kopia av server_name för att vi senare kommer använda den i mqtt_task
        params_d->mqtt_message_cv.notify_one(); //Notifar mqtt att vi behöver subsribra till serverns topics som vi
        logfile << "Connected to server: " << server_name << std::endl;
        app_state = AppState::ServerDetails;
        screen.PostEvent(ftxui::Event::Custom); //säger till att en uppdaterign skett vilket kommer köra renderingen igen
    });

    auto server_details_menu = MakeServerDetails(&server_name_, [&](const std::string& message) {
        logfile << "got message: " << message << std::endl;
        //skicka till mqtt
        {
            logfile << "trying to send message: " << message << std::endl;
            std::lock_guard<std::mutex> lock(sending_msg_mutex);
            sending_msg_que.push(message);
        }
        screen.PostEvent(ftxui::Event::Custom);
    });

    //har hand om olika komponenter i renderingen, Lägg till nya menyer här
    int current_tab = 0;
    auto tab_container = Container::Tab(
        {
            enter_name_menu, // 0
            server_overview_menu, // 1
            server_details_menu, // 2
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
            case AppState::ServerDetails:
                current_tab = 2;
                break;
            default:
                return text("Invalid state!") | color(Color::Red);
        }
        return tab_container->Render();
    });

    screen.Loop(main_renderer);

    return 0;
}
