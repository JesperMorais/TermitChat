#include <iostream>
#include <thread>
#include <chrono>

#include <MQTTClient.h>

using namespace std;

#define ADDRESS "tcp://localhost:1883"
#define QOS 2
#define TOPIC "server/data"
#define CLIENTID "testCLient"


void client_task() {

    int rc;
    MQTTClient client;

    MQTTClient_create(&client, ADDRESS, CLIENTID, 0 , NULL);
    MQTTClient_connectOptions conn_opts = MQTTClient_connectOptions_initializer;

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
    //UI för att sätta CLIENTNAMN -> leder till att clienten får ett unikt ID
    //UI & MQTT hitta serverar under subscribe - server/announcment
    //UI Kunna välja bland olika server och connecta till dem.
    cout << "Starting client..." << endl;
    client_task(); //Starta thread istället

    return 0;
}
