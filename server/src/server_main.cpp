#include <iostream>
#include <asio.hpp>
#include <ftxui/component/component.hpp>
#include <ftxui/component/screen_interactive.hpp>
#include <thread>
#include <mutex>
#include <queue>
#include "debug.hpp"
#include "asio_task.hpp"
#include "global_params.hpp"
//TODO: Splitta i filer, ha UI i main Men nätverk i en annan fil
//TODO: FIXA BROKER
//TODO: FIXA CLIENT PÅ SERVER SAMT CLIENT
using namespace ftxui;


int main() {
    if (getenv("IN_NEW_TERMINAL") == nullptr) {
    // Relaunch the program in a new terminal
        std::string command = "start cmd.exe /k \"set IN_NEW_TERMINAL=1 && build\\server\\debug\\server\"";
        std::system(command.c_str());
        return 0;
    }
    auto screen = ScreenInteractive::TerminalOutput();

    auto input_box = Input(&input_content, "Skriv ditt meddelande...");

    auto send_button = Button("Skicka", [&] {
        if (!input_content.empty()) {
            {
                std::lock_guard<std::mutex> lock(data_mutex);
                message_queue.push(input_content);
            }
            input_content = ""; // Clear input box
        } else {
            add_debug_message("Cannot send empty message");
        }
    }) | color(Color::Wheat1) | bold | border | bgcolor(Color::Black);

    auto container = Container::Vertical({
        send_button,
        input_box,
    });

auto renderer = Renderer(container, [&] {
    std::vector<Element> debug_elements;
    {
        std::lock_guard<std::mutex> lock(debug_mutex);
        for (const auto& msg : debug_messages) {
            debug_elements.push_back(text(msg) | color(Color::Yellow));
    }
        }
    // Return the UI layout (rebuilt completely on each render)
    return vbox({
        text("TermitChat") | bold | borderStyled(Color::BlueViolet) | color(Color::Green3Bis) | center | flex | blink, // Single Header
        separator(),
        text("Received: " + recv_data) | dim,                        // Received data
        separator(),                       // Display the message queue
        container->Render(),
        separator(),
        vbox({
            text("Debug Messages:") | bold | color(Color::Red),          // Debug Header
            vbox(debug_elements) | border,                              // Debug message panel                                         // Input box and send button
        }) | border,
    });
});

    std::thread asio_thread(asio_task);
    screen.Loop(renderer);
    asio_thread.join();
    return 0;
}
