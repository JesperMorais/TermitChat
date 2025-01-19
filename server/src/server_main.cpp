#include <iostream>
#include <asio.hpp>
#include <ftxui/component/component.hpp>
#include <ftxui/dom/elements.hpp>
#include <ftxui/component/screen_interactive.hpp>
#include <thread>
#include <mutex>
#include <queue>
#include "debug.hpp"
#include "global_params.hpp"
#include "mqtt_client.hpp"
#include <algorithm>

using namespace ftxui;

int main() {
    auto screen = ScreenInteractive::TerminalOutput();

    // Relaunch the program in a new terminal if not already in one
    if (getenv("IN_NEW_TERMINAL") == nullptr) {
        std::string command = "start cmd.exe /k \"set IN_NEW_TERMINAL=1 && build\\server\\debug\\server\"";
        std::system(command.c_str());
        return 0;
    }

    //TODO: ADD PASSWORD COMPATIBILITY
    // Step 1: Server Name Input Screen
    bool server_name_entered = false;

    auto server_input = Input(&input_content_server_name, "Skriv servernamn...");
    auto start_button = Button("Starta", [&] {
        if (!input_content_server_name.empty()) {
            server_name_entered = true;
            serverName = input_content_server_name;
            add_debug_message("Ansluter till server: " + input_content_server_name);
            screen.ExitLoopClosure()();
        } else {
            add_debug_message("Servernamn kan inte vara tomt!");
        }
    }) | color(Color::Green) | bold | border | bgcolor(Color::Black);

    auto startup_container = Container::Vertical({
        server_input,
        start_button,
    });

    auto startup_renderer = Renderer(startup_container, [&] {
        std::vector<Element> debug_elements;
        {
            std::lock_guard<std::mutex> lock(debug_mutex);
            for (const auto& msg : debug_messages) {
                debug_elements.push_back(text(msg) | color(Color::Yellow));
            }
        }
        return vbox({
            text("Välkommen till TermitChat!") | bold | center | color(Color::Blue),
            separator(),
            text("Ange servernamnet för att fortsätta:"),
            server_input->Render() | border,
            start_button->Render(),
            separator(),
            vbox({
                text("Debug Messages:") | bold | color(Color::Red),
                vbox(debug_elements) | border,
            }),
        });
    });

    // Display the startup screen
    screen.Loop(startup_renderer);

    // Exit if the server name was not entered
    if (!server_name_entered) {
        std::cerr << "Startup avbruten. Ingen server angiven." << std::endl;
        return -1;
    }
    
    //TODO:REWORK UI TO HAVE PROPPER SERVER UI 
    //SHOW ACTIVE USERS
    //CHANGE PASSWORD
    // Step 2: Main Chat UI
    auto input_box = Input(&input_content, "Skriv ditt meddelande...");
    auto send_button = Button("Skicka", [&] {
        if (!input_content.empty()) {
            {
                std::lock_guard<std::mutex> lock(data_mutex);
                message_queue.push(input_content);
            }
            input_content.clear(); // Clear the input box
        } else {
            add_debug_message("Cannot send empty message");
        }
    }) | color(Color::Wheat1) | bold | border | bgcolor(Color::Black);

    auto container = Container::Vertical({
        input_box,
        send_button,
    });

    auto renderer = Renderer(container, [&] {
        std::vector<Element> debug_elements;
        {
            std::lock_guard<std::mutex> lock(debug_mutex);
            for (const auto& msg : debug_messages) {
                debug_elements.push_back(text(msg) | color(Color::Yellow));
            }
        }
        return vbox({
            text("TermitChat") | bold | borderStyled(Color::BlueViolet) | color(Color::Green3Bis) | center | flex | blink,
            separator(),
            text("Hostar server: " + input_content_server_name) | bold | color(Color::Cyan),
            text("Activa Clienter: david, Jesper") | dim | border | flex,
            separator(),
            container->Render(),
            separator(),
            vbox({
                text("Debug Messages:") | bold | color(Color::Red),
                vbox(debug_elements) | border,
            }),
        });
    });

    // Start networking tasks
    std::thread mqtt_thread(mqtt_task);
    //std::thread asio_thread(asio_task);

    // Display the main UI
    screen.Loop(renderer);


    mqtt_thread.join();
    //asio_thread.join();

    return 0;
}