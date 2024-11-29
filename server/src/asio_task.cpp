#include "global_params.hpp"
#include "debug.hpp"
#include "asio_task.hpp"

using namespace std;

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