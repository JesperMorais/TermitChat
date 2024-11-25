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
string cannotSendData;
bool isClientConnected = false;
mutex data_mutex;
queue<string> message_queue;

void handle_client(tcp::socket& socket){
    //ta emot medelanden
    //skicka medelande i loop ta emot från kö
    try{

        char data[100];
        asio::error_code error;

        size_t length = socket.read_some(asio::buffer(data), error);
        
        if (error == asio::error::eof) {
            std::cout << "Client disconnected during handshake." << std::endl;
            return;
        }
        if (error) {
            throw asio::system_error(error);
        }

        string recv_message(data, length);
        cout << "recv data from client : " << recv_message << endl;

        cout << "Entering write loop" << endl;

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
                cout << "send message to client: " << message << endl;
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
        cout << "Wating for client to connect" << endl;
        acceptor.accept(socket);
        cout << "Client connected!" << endl;

        handle_client(socket);
        }catch(std::exception& e){
            cerr << "Server error: " << e.what() << std::endl;
            cout << "Retrying connection in 5 seconds..." << std::endl;
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
            cannotSendData = "Input is empty";
        }
    });

    auto container = Container::Vertical({
        send_button,
        input_box,
    });

    auto renderer = Renderer(container, [&] {
        std::vector<Element> queue_elements;
        {
            std::lock_guard<std::mutex> lock(data_mutex);
            std::queue<std::string> temp_queue = message_queue; // Copy queue for display
            while (!temp_queue.empty()) {
                queue_elements.push_back(text(temp_queue.front()));
                temp_queue.pop();
            }
        }

        return vbox({
            text("TermitChat") | bold | border | color(Color::Green3Bis),
            separator(),
            text("Received: " + recv_data) | dim,
            text(cannotSendData.empty() ? "" : ("Error: " + cannotSendData)) | color(Color::Red),
            separator(),
            text("Current message queue") | color(Color::Blue),
            vbox(queue_elements) | border,
            container->Render(),
        });
    });

    std::thread asio_thread(asio_task);
    screen.Loop(renderer);
    asio_thread.join();
    return 0;
}
