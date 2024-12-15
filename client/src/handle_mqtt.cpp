#include "MQTTClient.h"
#include <iostream>
#include <string.h>
#include "menus/menu_state.hpp"

#define SERVER_ANNOUNCMENT_TOPIC "server/announcment"
#define OFFLINE_CLIENT_TOPIC "client/status"
#define QOS 2
#define KEEP_ALIVE_INTERVALS 20

#define BROKER_ADRESS ""
#define BROKER_PORT ""


MQTTClient mqttClient;
bool isClientConnected = false;
std::string clientUsername;

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
            }
    }
}

void mqtt_task(void* thread_para){

    thread_params* parms = (thread_params*)thread_para;
    {
        std::unique_lock lk(parms->m);
        //väntar tills usernameSet är satt
        parms->cv.wait(lk, [&] {return parms->usernameSet;});
    }

    clientUsername = parms->clientInput_username;  

    //MQTT Creat wants const char* not a string so a covertion is needed
    const char* c_str_clientid = parms->mqttClient_ID.c_str();
    
    if (MQTTClient_create(&mqttClient, BROKER_ADRESS, c_str_clientid, MQTTCLIENT_PERSISTENCE_NONE, NULL) != MQTTCLIENT_SUCCESS) {
        std::runtime_error("Failed to create MQTT client");
    }

    //STUCK IN HERE WHILE WE CONNECT, MIGHT NEED TO FIX THIS
    mqtt_connect();
    
    //SUBSCRIBA TILL RELEVANT TOPIC
}