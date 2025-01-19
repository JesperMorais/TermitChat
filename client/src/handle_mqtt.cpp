#include "MQTTClient.h"
#include <iostream>
#include <string.h>
#include <thread>
#include <chrono>
#include "menus/menu_state.hpp"
#include "handle_mqtt.hpp"
#include "global_params.hpp"

#define SERVER_ANNOUNCMENT_TOPIC "/server/announcments/#"

const char* SERVER_ANNOUNCMENT_TOPIC_PREFIX = "/server/announcments/";
const size_t PREFIX_LENGTH = strlen(SERVER_ANNOUNCMENT_TOPIC_PREFIX);

#define OFFLINE_CLIENT_TOPIC "client/status"
#define QOS 2
#define KEEP_ALIVE_INTERVALS 20

#define BROKER_ADRESS "mqtt://127.0.0.1:1883"

MQTTClient mqttClient;
bool isClientConnected = false;
std::string clientUsername;

/*
TOPIC DEFINES HERE:

-- SERVER --
/server/announcments/# - ALL Servers will announce themselves on this topic and can be discovered by clients with retain flag set so they only have to send once
/server/n/clients - List of clients connected to server n
/server/n/chat - Chat messages for server n

-- CLIENT --
/client/n/status - Status of client n (online/offline)
/client/n/chat - Chat messages for client n (private messages)

-- GENERAL --
/clients - List of all clients connected to any server
/chat - Chat messages for all clients


*/ 

void handle_server_topic(MQTTClient_message *message){
    //Förväntas en payload som ser ut som {server_name: "server1", "time_stamp": "2021-09-01 12:00:00"}
    string payload = (char*)message->payload;
    string server_name = payload.substr(payload.find("server_name: ") + 13, payload.find(",") - 13);
    server_list.push_back(server_name);
}

// Callback function for connection lost
void connlost(void *context, char *cause) {
    std::cerr << "Connection lost: " << cause << std::endl;
    isClientConnected = false;
    mqtt_connect(); // Reconnect
}

// Callback function for message arrival
int msgarrvd(void *context, char *topicName, int topicLen, MQTTClient_message *message) {

    if(strncmp(topicName, SERVER_ANNOUNCMENT_TOPIC_PREFIX, PREFIX_LENGTH) == 0){
        handle_server_topic(message);
    }
    std::cout << "Message arrived on topic: " << topicName << std::endl;
    std::cout << "Message: " << (char*)message->payload << std::endl;
    MQTTClient_freeMessage(&message);
    MQTTClient_free(topicName);
    return 1;
}

// Callback function for message delivery completion
void delivered(void *context, MQTTClient_deliveryToken dt) {
    std::cout << "Message with token " << dt << " delivered." << std::endl;
}

void mqtt_connect(){
    MQTTClient_willOptions will_opts = MQTTClient_willOptions_initializer;
        will_opts.topicName = OFFLINE_CLIENT_TOPIC;
        will_opts.message = "offline";
        will_opts.qos = QOS;

        MQTTClient_connectOptions conn_opts = MQTTClient_connectOptions_initializer;
        conn_opts.will = &will_opts;
        conn_opts.keepAliveInterval = KEEP_ALIVE_INTERVALS;
        conn_opts.cleansession = 1;

        while (!isClientConnected) {
            int rc = MQTTClient_connect(mqttClient, &conn_opts);
            if (rc != MQTTCLIENT_SUCCESS) {
                std::cerr << "Failed to connect, retrying... (Error: " << rc << ")" << std::endl;
                std::this_thread::sleep_for(std::chrono::seconds(5));
            } else {
                isClientConnected = true;
                std::cout << "Connected to MQTT broker" << std::endl;
                break;
            }
    }
}

void subscribe_to_topic(){
    int rc = MQTTClient_subscribe(mqttClient, SERVER_ANNOUNCMENT_TOPIC, QOS);
    if (rc != MQTTCLIENT_SUCCESS) {
        std::cerr << "Failed to subscribe to topic: " << SERVER_ANNOUNCMENT_TOPIC << std::endl;
    }
    std::cout << "Subscribed to topic: " << SERVER_ANNOUNCMENT_TOPIC << std::endl;
}

void mqtt_task(void* thread_para){

    std::cout << "MQTT Task started" << std::endl;
    //väntar tills usernameSet är satt innan vi sätter mqtt username
    thread_params* parms = (thread_params*)thread_para;
    {
        std::unique_lock lk(parms->m);
        parms->cv.wait(lk, [&] {return parms->usernameSet;});
    }

    clientUsername = parms->clientInput_username;  

    //MQTT Create wants const char* not a string so a covertion is needed
    const char* c_str_clientid = parms->mqttClient_ID.c_str();
    
    if (MQTTClient_create(&mqttClient, BROKER_ADRESS, c_str_clientid, MQTTCLIENT_PERSISTENCE_NONE, NULL) != MQTTCLIENT_SUCCESS) {
        std::runtime_error("Failed to create MQTT client");
    }

    //STUCK IN HERE WHILE WE CONNECT, MIGHT NEED TO FIX THIS
    mqtt_connect();

    //client, context/userdefines, connectionLost, messageArrived, deliveryComplete
    MQTTClient_setCallbacks(mqttClient, parms, connlost, msgarrvd, delivered);
    
    //this function subscribes to pre determined topics the user will beable to see / use
    subscribe_to_topic();

    //Sleep to keep task alive for now
    while(true){
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
    //SUBSCRIBA TILL RELEVANT TOPIC
}