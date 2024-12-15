#include "MQTTClient.h"
#include <iostream>
#include <string.h>
#include "menus/menu_state.hpp"

#define SERVER_ANNOUNCMENT_TOPIC "server/announcment"
#define BROKER_ADRESS ""
#define BROKER_PORT ""


MQTTClient mqttClient;
bool isClientConnected;
std::string clientUsername;


void mqtt_task(void* thread_para){

    thread_params* parms = (thread_params*)thread_para;
    {
        std::unique_lock lk(parms->m);
        //väntar tills usernameSet är satt
        parms->cv.wait(lk, [&] {return parms->usernameSet;});
    }

    clientUsername = parms->clientInput_username;  

    

    //VÄNTA PÅ ATT FTXUI HAR FÅTT CLIENTNAMN!
    //cv.wait

    //SKAPA client inom MQTT Scopen
    //Skapa alla variabler som kommer behövas
    //Initera Client
    //Connecta Client till MOSQUITTO

    //SUBSCRIBA TILL RELEVANT TOPIC
}