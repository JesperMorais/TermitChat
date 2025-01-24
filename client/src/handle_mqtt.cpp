#include "MQTTClient.h"
#include <iostream>
#include <string.h>
#include <thread>
#include <chrono>
#include "menus/menu_state.hpp"
#include "handle_mqtt.hpp"
#include "global_params.hpp"
#include <MQTTAsync.h>


#define SERVER_ANNOUNCMENT_TOPIC "/server/announcments/#"
#define SERVER_CHAT "/server/n/chat"
const char* SERVER_TOPIC_PREFIX = "/server/";
const char* CHAT_PREFIX = "/chat";

const char* SERVER_ANNOUNCMENT_TOPIC_PREFIX = "/server/announcments/";
const size_t PREFIX_LENGTH = strlen(SERVER_ANNOUNCMENT_TOPIC_PREFIX);

#define OFFLINE_CLIENT_TOPIC "client/status"
#define QOS 2
#define KEEP_ALIVE_INTERVALS 20

string CURRENT_SERVER_CHAT_TOPIC; // UPDATERAS NÄR VI CONNECTAR TILL EN SERVER

#define BROKER_ADRESS "mqtt://127.0.0.1:1883"

MQTTClient mqttClient;
bool isClientConnected = false;
std::string clientUsername;
bool chat_has_set = false;

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

void sub_to_current_server_task(void* params){
    logfile << "Sub to current server task started" << std::endl;
    thread_params* parms = (thread_params*)params;
    std::string current_server_name;
    while(true){
        logfile << "Waiting for new server to subscribe to" << std::endl;
        {
            std::unique_lock lk(parms->m);
            parms->mqtt_message_cv.wait(lk, [&] {return parms->current_server_name != current_server_name;}); //kollar så vi inte subbar till samma server igen
        }
        //unsubscribe from old server
        if(current_server_name != ""){
            std::string topic = "/server/" + current_server_name + "/#";
            int rc = MQTTClient_unsubscribe(mqttClient, topic.c_str());
            if (rc != MQTTCLIENT_SUCCESS) {
                logfile << "Failed to unsubscribe from topic: " << topic << std::endl;
            }
            logfile << "Unsubscribed from topic: " << topic << std::endl;
        }

        current_server_name = parms->current_server_name;
        logfile << "Subscribing to server: " << current_server_name << std::endl;
        std::string topic = "/server/" + current_server_name + "/#";
        int rc = MQTTClient_subscribe(mqttClient, topic.c_str(), QOS);
        if (rc != MQTTCLIENT_SUCCESS) {
            logfile << "Failed to subscribe to topic: " << topic << std::endl;
        }
        logfile << "Subscribed to topic: " << topic << std::endl;
        CURRENT_SERVER_CHAT_TOPIC = SERVER_TOPIC_PREFIX + current_server_name + CHAT_PREFIX;
        logfile << "made chat topic: " << CURRENT_SERVER_CHAT_TOPIC << std::endl;

        rc = MQTTClient_subscribe(mqttClient, CURRENT_SERVER_CHAT_TOPIC.c_str(), QOS);
        if (rc != MQTTCLIENT_SUCCESS) {
            logfile << "Failed to subscribe to topic: " << CURRENT_SERVER_CHAT_TOPIC << std::endl;
        }else{
            logfile << "Subscribed to topic: " << CURRENT_SERVER_CHAT_TOPIC << std::endl;
        }
        logfile << "current chat define is now set to:" << CURRENT_SERVER_CHAT_TOPIC << std::endl;
    }
}

void handle_chat_messages(MQTTClient_message *message){
    logfile << "Inside Handling chat message" << std::endl;
    string paylod = (char*)message->payload;
    //mutex que och lägg in data i que
    {
        logfile << "Pushing chat message to que" << std::endl;
        lock_guard<std::mutex> lock(chat_que_mutex);
        if(chat_que.size() >= 10){
            chat_que.pop();
        }
        chat_que.push(paylod);
        logfile << "Pushed chat message to que:" << (string)paylod <<std::endl;
    }
    logfile << "Handling chat message: " << paylod << std::endl;
}

void handle_server_topic(MQTTClient_message *message){
    //Förväntas en payload som ser ut som {server_name: "server1", "time_stamp": "2021-09-01 12:00:00"}
    logfile << "Handling server topic" << std::endl;
    if(message->payloadlen < 1){
        return;
    }
    //kolla om den redan finns
    string payload = (char*)message->payload;
    string server_name = payload.substr(payload.find("server_name: ") + 13, payload.find(",") - 14);

    //we only want to check trough the list if its not empty
    if(!server_list.empty()){
        logfile << "Server list is not empty" << std::endl;
    for(auto server: server_list){
            if(server == server_name){
                logfile << "servername: " << server_name << " already in list" << std::endl;
                return;
            }
        }
    }
    logfile << "Adding server: " << server_name << " to list" << std::endl;
    {
        lock_guard<std::mutex> lock(server_list_mutex);
        server_list.push_back(server_name);
    }
}

// Callback function for connection lost
void connlost(void *context, char *cause) {
    logfile << "Connection lost: " << cause << std::endl;
    isClientConnected = false;
    mqtt_connect(); // Reconnect
}

// Callback function for message arrival
int msgarrvd(void *context, char *topicName, int topicLen, MQTTClient_message *message) {
    if(strncmp(topicName, SERVER_ANNOUNCMENT_TOPIC_PREFIX, PREFIX_LENGTH) == 0){
        handle_server_topic(message);
    }else if(!CURRENT_SERVER_CHAT_TOPIC.empty()){
        logfile << "currnet chat topic not empty" << std::endl;
        if(strncmp(topicName, CURRENT_SERVER_CHAT_TOPIC.c_str(), CURRENT_SERVER_CHAT_TOPIC.length()) == 0){
            logfile << "Handling chat message" << std::endl;
            handle_chat_messages(message);
        }
    }else{
        logfile << "the topic was" << topicName << "and the current chat topic is: " << CURRENT_SERVER_CHAT_TOPIC << std::endl;
    }
    logfile << "Message: " << (char*)message->payload << std::endl;
    MQTTClient_freeMessage(&message);
    MQTTClient_free(topicName);
    return 1;
}

// Callback function for message delivery completion
void delivered(void *context, MQTTClient_deliveryToken dt) {
    logfile << "Message with token " << dt << " delivered." << std::endl;
}

void mqtt_connect(){

        logfile << MQTTClient_setCallbacks(mqttClient, NULL, connlost, msgarrvd, delivered) << std::endl;

        MQTTClient_connectOptions conn_opts = MQTTClient_connectOptions_initializer;
        conn_opts.keepAliveInterval = KEEP_ALIVE_INTERVALS;
                

        while (!isClientConnected) {
            int rc = MQTTClient_connect(mqttClient, &conn_opts);
            if (rc != MQTTCLIENT_SUCCESS) {
                logfile << "Failed to connect, retrying... (Error: " << rc << ")" << std::endl;
                std::this_thread::sleep_for(std::chrono::seconds(5));
            } else {
                isClientConnected = true;
                logfile << "Connected to MQTT broker succecfully" << std::endl;
                break;
            }
    }
}

void subscribe_to_topic(){
    int rc = MQTTClient_subscribe(mqttClient, SERVER_ANNOUNCMENT_TOPIC, QOS);
    if (rc != MQTTCLIENT_SUCCESS) {
        logfile << "Failed to subscribe to topic: " << SERVER_ANNOUNCMENT_TOPIC << std::endl;
    }
    logfile << "Subscribed to topic: " << SERVER_ANNOUNCMENT_TOPIC << std::endl;
}

void mqtt_task(void* thread_para){

    logfile << "MQTT Task started" << std::endl;
    //väntar tills usernameSet är satt innan vi sätter mqtt username
    thread_params* parms = (thread_params*)thread_para;
    {
        std::unique_lock lk(parms->m);
        parms->cv.wait(lk, [&] {return parms->usernameSet;});
    }

    
    clientUsername = parms->clientInput_username;  
    //MQTT Create wants const char* not a string so a covertion is needed
    const char* c_str_clientid = parms->mqttClient_ID.c_str();

    logfile << "Client username set to: " << clientUsername << std::endl;
    logfile << "Client ID set to: " << c_str_clientid << std::endl;

    if (MQTTClient_create(&mqttClient, BROKER_ADRESS, c_str_clientid, MQTTCLIENT_PERSISTENCE_NONE, NULL) != MQTTCLIENT_SUCCESS) {
        logfile << "Failed to create MQTT client" << std::endl;
    }
    else {
        logfile << "MQTT client created" << std::endl;
    }
    //STUCK IN HERE WHILE WE CONNECT, MIGHT NEED TO FIX THIS
    mqtt_connect();

    thread sub_to_server(sub_to_current_server_task, parms);
    sub_to_server.detach();

    //this function subscribes to pre determined topics the user will beable to see / use
    subscribe_to_topic();

    logfile << "MQTT yeild loop starting" << std::endl;

    while(true){
        MQTTClient_yield();
    }
    //SUBSCRIBA TILL RELEVANT TOPIC
}