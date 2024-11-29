#include <iostream>
#include <asio.hpp>
#include <thread>
#include <chrono>

#include <MQTTClient.h>

using asio::ip::tcp;

using namespace std;

#define ADDRESS "tcp://localhost:1884"
#define QOS 2
#define TOPIC "server/data"
#define CLIENTID "testCLient"

#define USERNAME "jesper"
#define PASSWORD "morais"

void client_task(const std::string& server_ip, const std::string& server_port) {
    asio::io_context io_context;
    int rc;
    MQTTClient client;

    MQTTClient_create(&client, ADDRESS, CLIENTID, 0 , NULL);
    MQTTClient_connectOptions conn_opts = MQTTClient_connectOptions_initializer;
    conn_opts.username = USERNAME;
    conn_opts.password = PASSWORD;

    cout << "trying to connect to MQTT" << endl;
    if((rc = MQTTClient_connect(client, &conn_opts)) != MQTTCLIENT_SUCCESS) {
        printf("Failed to connect, return code %d\n", rc);
        exit(-1);
    }
    else {
        cout << "CONNECTED TO MQTT BROKER!!!" << endl;
    }

    MQTTClient_subscribe(client, TOPIC, QOS);

    while (true) {
        try {
            // Resolve server address and port
            tcp::resolver resolver(io_context);
            auto endpoints = resolver.resolve(server_ip, server_port);

            // Create and connect the socket
            tcp::socket socket(io_context);
            asio::connect(socket, endpoints);
            std::cout << "Connected to server!" << std::endl;

            // Send initial "Hello, Server" message
            std::string hello_message = "Hello, Server!";
            asio::write(socket, asio::buffer(hello_message));
            std::cout << "Sent: " << hello_message << std::endl;

            // Continuous listening for server messages
            while (true) {
                char data[1024];
                asio::error_code error;

                // Read data from the server
                size_t length = socket.read_some(asio::buffer(data), error);

                if (error == asio::error::eof) {
                    std::cout << "Server closed the connection." << std::endl;
                    break; // Exit inner loop to reconnect
                } else if (error) {
                    throw asio::system_error(error);
                }

                // Process the received data
                std::cout << "Received from server: " << std::string(data, length) << std::endl;
            }

        } catch (std::exception& e) {
            std::cerr << "Exception: " << e.what() << std::endl;
        }

        // Wait before retrying the connection
        std::cout << "Retrying connection in 5 seconds..." << std::endl;
        std::this_thread::sleep_for(std::chrono::seconds(5));
    }
}

int main() {
    const std::string server_ip = "192.168.10.199";
    const std::string server_port = "12345";

    std::cout << "Starting client..." << std::endl;
    client_task(server_ip, server_port);

    return 0;
}
