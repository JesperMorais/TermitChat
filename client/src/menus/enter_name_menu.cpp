#include "enter_name_menu.hpp"
#include <ftxui/component/component.hpp>
#include <ftxui/component/screen_interactive.hpp> 
#include <ftxui/dom/elements.hpp>
#include "global_params.hpp"
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


int selected_server = 0;
Component MakeServerOverview(function<void(const string&)> on_connect) {
    // Skapar Radiobox med pekare till den globala server_list och selected_server
    auto server_radio = Radiobox(&server_list, &selected_server);

    logfile << "in Make server overview" << std::endl;
    // Knapp för att ansluta till vald server
    auto connect_button = Button("Anslut", [=] {
        // Lås mutexen för trådsäker åtkomst till server_list
        std::lock_guard<std::mutex> lock(server_list_mutex);
        if(server_list.empty()){
            logfile << "Servers is empty" << std::endl;
        } else if(selected_server >= server_list.size()){
            logfile << selected_server << " is bigger than the size of the server list" << std::endl;
        } else {
            // Anropa callbacken med namnet på den valda servern
            on_connect(server_list[selected_server]);
        }
    });

    // Skapa en container med radioknappen och anslut-knappen
    auto container = Container::Vertical({
        server_radio,
        connect_button,
    });

    // Renderer för UI-komponenten
    auto renderer = Renderer(container, [=] {
        // Lås mutexen under rendering för att skydda åtkomst av server_list
        std::lock_guard<std::mutex> lock(server_list_mutex);
        return vbox({
            text("Tillgängliga servrar:") | bold | center,
            separator(),
            server_radio->Render() | border | flex | color(Color::Green3Bis),
            separator(),
            connect_button->Render(),
        }) | border | center;
    });

    return renderer;
}
// Funktion för att hämta aktuella meddelanden
std::vector<std::string> GetChatMessages() {
    std::vector<std::string> messages;
    std::lock_guard<std::mutex> lock(chat_que_mutex);
    std::queue<std::string> temp = chat_que; // Kopiera kön
    while (!temp.empty()) {
        messages.push_back(temp.front());
        temp.pop();
    }
    return messages;
}

// Funktion för att skapa meddelanderutan
ftxui::Component MakeChatBox() {
    return ftxui::Renderer([&] {
        auto messages = GetChatMessages();
        std::vector<ftxui::Element> message_elements;
        for (const auto& msg : messages) {
            message_elements.push_back(ftxui::text(msg) | color(Color::Yellow2));
        }

        return ftxui::vbox({
                   ftxui::text("Meddelanden") | ftxui::bold,
                   ftxui::separator(),
                   ftxui::vbox(message_elements) | ftxui::frame | ftxui::size(ftxui::HEIGHT, ftxui::EQUAL, 20),
               }) |
               ftxui::border | ftxui::size(ftxui::WIDTH, ftxui::EQUAL, 50);
    });
}

string chat_input;

Component MakeServerDetails(const std::string* serverName, std::function<void(const std::string&)> on_connect) {
    using namespace ftxui;

    auto input = Input(&chat_input, "Skriv meddelande...");

    // Skapa en knapp med callback för att ansluta

    // Skapa en knapp med callback för att ansluta
    auto connect_button = Button("skicka", [=] {
        std::string message = chat_input;
        logfile << "trying to send message: " << message << std::endl;
        if (!message.empty()) {
            logfile << "message not empty: " << message << std::endl;
            try {
                logfile << "calling on_connect" << std::endl;
                on_connect(message); // Pass by value or const reference
                logfile << "called on_connect successfully" << std::endl;
                chat_input = "";
            } catch (const std::exception& e) {
                logfile << "Exception caught: " << e.what() << std::endl;
            } catch (...) {
                logfile << "Unknown exception caught" << std::endl;
            }
        } else {
            logfile << "no empty messages allowed" << std::endl;
        }
    });

    // Skapa meddelanderutan
    auto chat_box = MakeChatBox();

    // Skapa en container med vertikal layout
    auto container = Container::Vertical({
        connect_button,
        input,
        chat_box, // Lägg till meddelanderutan här
    });

    // Renderer för att bygga utseendet på serverdetaljskärmen
    auto renderer = Renderer(container, [=] {
        return vbox({
                   text("Server") | bold | center | color(Color::Blue3Bis),
                   separator(),
                   text("Current Server: " + *serverName) | center | color(Color::Green3Bis) | blink | bold,
                    separator(),
                    input->Render() | border | center,
                   // Här kan du lägga till fler serverdetaljer vid behov
                   separator(),
                   connect_button->Render() | center,
                   separator(),
                   chat_box->Render() // Rendera meddelanderutan
               }) |
               border | center;
    });

    return renderer;
}