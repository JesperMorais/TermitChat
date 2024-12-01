#include "mqtt_client.hpp"
#include "debug.hpp"
#include <thread>
#include <format>
#include "global_params.hpp"
bool isClientConnected = false;


MQTTClient create_mqtt_client(std::string adress){    
    MQTTClient client;
    
    if(MQTTClient_create(&client, ADDRESS, CLIENTID, MQTTCLIENT_PERSISTENCE_NONE, NULL) != MQTTCLIENT_SUCCESS){
        add_debug_message("Error creating mqtt client");
    }
    add_debug_message("Created MQTT Client!");
    return client;
}

void connect_mqtt_client(MQTTClient *client){

    MQTTClient_connectOptions conn_opts = MQTTClient_connectOptions_initializer;

    if(MQTTClient_connect(*client, &conn_opts) != MQTTCLIENT_SUCCESS){
        add_debug_message("GICK INTE ATT CONNECTA MED MQTTClienten");
    }else{
        isClientConnected = true;
    }

}

void publish_server_online(MQTTClient *client){
    //ID:SERVERNAMN:BOOLIFPASSWORD

    bool reqPassword = false;
    string message = string(1, serverID) + ":" + serverName + ":" + to_string(reqPassword);

    const char* message_cstr = message.c_str();


    MQTTClient_publish(client, TOPIC, strlen(message_cstr), message_cstr, QOS, 0, NULL);

    string msg = (string)message;
    add_debug_message("Published message to topic: " + msg);
}

/// @brief Tar han om MQTT servern.
void mqtt_task(){
    serverID = '4'; // TODO: FIXA SEANRE
    MQTTClient client;
    while(!isClientConnected){
        client = create_mqtt_client(ADDRESS);
        connect_mqtt_client(&client);
    }
    
    publish_server_online(&client);
    while(1){
        MQTTClient_yield();
    }
}