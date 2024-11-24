#include <iostream>
#include <asio.hpp>

using asio::ip::tcp;

int main() {
    try {
        // Create an io_context object
        asio::io_context io_context;

        // Resolve the server address and port
        tcp::resolver resolver(io_context);
        auto endpoints = resolver.resolve(" ", "12345");

        // Create a socket and connect to the server
        tcp::socket socket(io_context);
        asio::connect(socket, endpoints);

        std::cout << "Connected to server!" << std::endl;

        // Message to send to the server
        std::string message = "Hello, Server!";
        asio::write(socket, asio::buffer(message));

        std::cout << "Sent: " << message << std::endl;

        // Buffer to store the server's response
        char data[1024];
        asio::error_code error;

        // Read the response from the server
        size_t length = socket.read_some(asio::buffer(data), error);
        if (error == asio::error::eof) {
            std::cout << "Server closed the connection." << std::endl;
        } else if (error) {
            throw asio::system_error(error);
        }

        std::cout << "Received: " << std::string(data, length) << std::endl;
    } catch (std::exception& e) {
        std::cerr << "Exception: " << e.what() << std::endl;
    }

    return 0;
}