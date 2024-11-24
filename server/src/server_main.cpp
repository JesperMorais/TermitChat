#include <iostream>
#include <asio.hpp>

using asio::ip::tcp;

int main() {
    try {
        // Create an io_context object
        asio::io_context io_context;

        // Create a TCP acceptor on port 12345
        tcp::acceptor acceptor(io_context, tcp::endpoint(tcp::v4(), 12345));

        std::cout << "Server is running on port 12345..." << std::endl;

        // Wait for a client to connect
        tcp::socket socket(io_context);
        acceptor.accept(socket);

        std::cout << "Client connected!" << std::endl;

        // Buffer to store received data
        char data[1024];
        asio::error_code error;

        // Read data from the client
        size_t length = socket.read_some(asio::buffer(data), error);
        if (error == asio::error::eof) {
            std::cout << "Client disconnected." << std::endl;
        } else if (error) {
            throw asio::system_error(error);
        }

        std::cout << "Received: " << std::string(data, length) << std::endl;

        // Echo the message back to the client
        asio::write(socket, asio::buffer(data, length));

        std::cout << "Message echoed back to client." << std::endl;
    } catch (std::exception& e) {
        std::cerr << "Exception: " << e.what() << std::endl;
    }

    return 0;
}