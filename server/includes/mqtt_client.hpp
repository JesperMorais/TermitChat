#pragma once

#include <iostream>
#include <MQTTClient.h>
#include <string>

#define ADDRESS "tcp://localhost:1883"
#define QOS 2
#define TOPIC "server/data"
#define TIPIC2 "server/ID/"
#define CLIENTID "testCLient"


extern bool isClientConnected;

/// @brief Skapar en MQTT client och inilizerar den
/// @param Tar emot addressen till brokern som den ska connecta till
///@return En MQTT CLient
MQTTClient create_mqtt_client(std::string adress);

/// @brief connectar clienten till brokern
/// @param client 
void connect_mqtt_client(MQTTClient *client);

/// @brief Skickar ut en annoucnment i "server/announcment" att server är online
/// @param client 
void publish_server_online(MQTTClient *client);

/// @brief Tar han om MQTT servern.
void mqtt_task();

