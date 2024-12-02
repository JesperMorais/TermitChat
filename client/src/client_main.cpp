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

}

int main() {

    std::cout << "Starting client..." << std::endl;
    client_task(server_ip, server_port);

    return 0;
}
