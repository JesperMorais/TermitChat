#include "mqtt_client.hpp"
#include "debug.hpp"
#include <thread>
#include <format>
#include "global_params.hpp"
bool isClientConnected = false;

/// @brief Skapar en MQTT client och inilizerar den
/// @param Tar emot addressen till brokern som den ska connecta till
///@return En MQTT CLient
MQTTClient create_mqtt_client(std::string adress){    
    MQTTClient client;
    
    if(MQTTClient_create(&client, ADDRESS, CLIENTID, MQTTCLIENT_PERSISTENCE_NONE, NULL) != MQTTCLIENT_SUCCESS){
        add_debug_message("Error creating mqtt client");
    }
    add_debug_message("Created MQTT Client!");
    return client;
}

/// @brief connectar clienten till brokern
/// @param client 
void connect_mqtt_client(MQTTClient *client){
    MQTTClient_connectOptions conn_opts = MQTTClient_connectOptions_initializer;
    if(MQTTClient_connect(*client, &conn_opts) != MQTTCLIENT_SUCCESS){
        add_debug_message("GICK INTE ATT CONNECTA MED MQTTClienten");
    }else{
        isClientConnected = true;
    }

}

/// @brief Skickar ut en annoucnment i "server/announcment" att server är online
/// @param client 
void publish_server_online(MQTTClient *client){
    string message = R"({"server_id": ")" + serverName + R"(", "status": "online"})";

    const char* message_cstr = message.c_str();

    MQTTClient_publish(client, TOPIC, strlen(message_cstr), message_cstr, QOS, 0, NULL);

    string msg = (string)message;
    add_debug_message("Published message to topic: " + msg);
}

/// @brief Tar han om MQTT servern.
void mqtt_task(){
    MQTTClient client;
    while(!isClientConnected){
        client = create_mqtt_client(ADDRESS);
        connect_mqtt_client(&client);
    }
    
    publish_server_online(&client);
    while(1){

    }
}