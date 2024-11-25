#include <iostream>
#include <asio.hpp>
#include <ftxui/component/component.hpp>
#include <ftxui/component/screen_interactive.hpp>
#include <thread>
#include <mutex>
#include <queue>

using asio::ip::tcp;
using namespace ftxui;
using namespace std;

string input_content;
string recv_data;
bool isClientConnected = false;
mutex data_mutex;
queue<string> message_queue;

vector<string> debug_messages;
mutex debug_mutex;

//You can't just cout debug log with ftxui so we need a new way
//We there for add all debug messages to a vector 
void add_debug_message(const string& message) {
    lock_guard<std::mutex> lock(debug_mutex);
    debug_messages.push_back(message);

    // Keep only the last 10 messages
    if (debug_messages.size() > 10) {
        debug_messages.erase(debug_messages.begin());
    }
}


void handle_client(tcp::socket& socket){
    //ta emot medelanden
    //skicka medelande i loop ta emot från kö
    try{

        char data[100];
        asio::error_code error;

        size_t length = socket.read_some(asio::buffer(data), error);
        
        if (error == asio::error::eof) {
            add_debug_message("Client disconnected during handshake.");
            return;
        }
        if (error) {
            throw asio::system_error(error);
        }

        string recv_message(data, length);
        if(recv_data != recv_message){
            recv_data = recv_message;
        }

        add_debug_message("recv data from client :" + recv_message);
        add_debug_message("Entering write loop");


        while(true){
            string message; // gonna hold the send message
            {
                lock_guard<mutex> lock(data_mutex);
                if(!message_queue.empty()) {
                    message = message_queue.front(); //Extract the message from the que
                    message_queue.pop(); // remove it to make room for other
                }
            }

            if(!message.empty()){ // is there a message in "message"
                asio::write(socket, asio::buffer(message), error);
                if(error){
                    throw asio::system_error(error);
                }
                add_debug_message("send message to client: " + message);
            } 
            this_thread::sleep_for(chrono::milliseconds(100));
        }
    }catch(std::exception& e){
        cerr << "error with client: " << e.what() << endl;
    }
}

void asio_task() {
    asio::io_context io_context;
    tcp::acceptor acceptor(io_context, tcp::endpoint(tcp::v4(), 12345));

    while (true) {
        try{

        tcp::socket socket(io_context);
        add_debug_message("Wating for client to connect");
        acceptor.accept(socket);
        add_debug_message("Client connected!");

        handle_client(socket);
        }catch(std::exception& e){
            cerr << "Server error: " << e.what() << std::endl;
            add_debug_message("Retrying connection in 5 seconds...");
            this_thread::sleep_for(std::chrono::seconds(5));
        }     
    }
}
   
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
