#include <iostream>
#include <thread>
#include <chrono>
#include <ftxui/component/component.hpp>
#include <ftxui/dom/elements.hpp>
#include <ftxui/component/screen_interactive.hpp>

#include <MQTTClient.h>
#include <MQTTAsync.h>
#include "handle_uuid.hpp"
#include "menus/menu_state.hpp"
#include "handle_mqtt.hpp"

using namespace std;
using namespace ftxui;


int main() {

    thread_params tp;

    if (getenv("IN_NEW_TERMINAL") == nullptr) {
        std::string command = "start cmd.exe /k \"set IN_NEW_TERMINAL=1 && build\\client\\debug\\client\"";
        std::system(command.c_str());
        return 0;
    }
    
    thread ftxui_task(rund_app, &tp);
    thread mqtt_task(mqtt_task, &tp);
    ftxui_task.join();


    return 0;
}

// class MQTTManager {
//     MQTTClient client;
//     std::mutex mqttMutex;
//     bool isConnected;

// public:
//     MQTTManager(char* address, char* clientID) : isConnected(false) {
//         if (MQTTClient_create(&client, address, clientID, MQTTCLIENT_PERSISTENCE_NONE, NULL) != MQTTCLIENT_SUCCESS) {
//             throw std::runtime_error("Failed to create MQTT client");
//         }

//         // Koppla en disconnect-callback
//         MQTTClient_setDisconnected(client, NULL, this->onDisconnect);
//     }

//     void connect() {
//         MQTTClient_willOptions will_opts = MQTTClient_willOptions_initializer;
//         will_opts.topicName = "client/status";
//         will_opts.message = "offline";
//         will_opts.qos = 1;
//         will_opts.retained = 1;

//         MQTTClient_connectOptions conn_opts = MQTTClient_connectOptions_initializer;
//         conn_opts.will = &will_opts;
//         conn_opts.keepAliveInterval = 20;
//         conn_opts.cleansession = 1;

//         while (!isConnected) {
//             std::lock_guard<std::mutex> lock(mqttMutex);
//             int rc = MQTTClient_connect(client, &conn_opts);
//             if (rc != MQTTCLIENT_SUCCESS) {
//                 std::cerr << "Failed to connect, retrying... (Error: " << rc << ")" << std::endl;
//                 std::this_thread::sleep_for(std::chrono::seconds(5));
//             } else {
//                 isConnected = true;
//                 std::cout << "Connected to MQTT broker" << std::endl;
//             }
//         }
//     }

//     void reconnect() {
//         connect();
//     }

//     ~MQTTManager() {
//         if (isConnected) {
//             MQTTClient_disconnect(client, 1000);
//         }
//         MQTTClient_destroy(&client);
//     }

//     static void onDisconnect(void* context, char* cause) {
//         std::cout << "Disconnected: " << (cause ? cause : "Unknown reason") << std::endl;
//         auto* manager = static_cast<MQTTManager*>(context);
//         manager->reconnect();
//     }
// };