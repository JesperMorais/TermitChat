#include "mqtt_client.hpp"
#include "debug.hpp"
#include <thread>
#include "global_params.hpp"
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <sstream>

using namespace boost::uuids;

bool isClientConnected = false;

int tries = 1;

MQTTClient create_mqtt_client(std::string adress){    
    MQTTClient client;
    
    if(MQTTClient_create(&client, ADDRESS, CLIENTID, MQTTCLIENT_PERSISTENCE_NONE, NULL) != MQTTCLIENT_SUCCESS){
        add_debug_message("Error creating mqtt client");
    }
    add_debug_message("Created MQTT Client!");
    return client;
}

void connect_mqtt_client(MQTTClient *client)
{
    MQTTClient_willOptions will_opts = MQTTClient_willOptions_initializer;
    will_opts.topicName = "client/status";
    will_opts.message = "offline";
    will_opts.qos = 1;
    will_opts.retained = 1;

    MQTTClient_connectOptions conn_opts = MQTTClient_connectOptions_initializer;
    conn_opts.will = &will_opts;
    conn_opts.keepAliveInterval = 20;   

    int rc;
    if ((rc = MQTTClient_connect(*client, &conn_opts)) != MQTTCLIENT_SUCCESS)
    {
        add_debug_message("MQTTClient_connect failed with error code: ");
    }
    else
    {
        add_debug_message("MQTT Client Connected!");
        MQTTClient_subscribe(*client, "/test", QOS);
        isClientConnected = true;
    }
}

void publish_server_online(MQTTClient *client){
    //Förväntas en payload som ser ut som {server_name: "server1", "time_stamp": "2021-09-01 12:00:00"}
    //TODO: FIXA REAL TIME STAMP
    string message = "{server_name: " + serverName + ", time_stamp: " + "2021-09-01 12:00:00}";
    const char* message_cstr = message.c_str();

    //FIXA TOPIC SÅ ATT DEN SÄGER /server/announcments/serverID

    string topic = "/server/announcments/" + serverName;
    const char* topic_cstr = topic.c_str();
    int count = 0;
    while (count < 5)
    {
        MQTTClient_publish(*client, topic_cstr, strlen(message_cstr), message_cstr, QOS, 1, NULL);
        count++;
        this_thread::sleep_for(chrono::milliseconds(1000));
    }

    MQTTClient_publish(*client, "/test", strlen("WASSAP TEST"), "WASSAP TEST", QOS, 1, NULL);
    string msg = (string)message;
    add_debug_message("Published message to topic: " + topic + " with message: " + msg);
}

void connlost(void *context, char *cause) {
    add_debug_message("Connection lost: " + (string)cause);
    isClientConnected = false;
}

// Callback function for message arrival
int msgarrvd(void *context, char *topicName, int topicLen, MQTTClient_message *message) {
    add_debug_message(">>> msgarrvd callback anropad!");
    add_debug_message("Message arrived with len " + to_string(topicLen));

    MQTTClient_freeMessage(&message);
    MQTTClient_free(topicName);
    return 1;
}

// Callback function for message delivery completion
void delivered(void *context, MQTTClient_deliveryToken dt) {
    add_debug_message("Message with token " + to_string(dt) + " delivered.");
}

/// @brief Tar han om MQTT servern.
void mqtt_task(){
    uuid id = random_generator()();

    // Konvertera UUID till en sträng
    stringstream ss;
    ss << id;
    string uuid_str = ss.str();
    
    serverID = uuid_str; // TODO: FIXA SEANRE
    MQTTClient client;
    while(!isClientConnected){

        this_thread::sleep_for(chrono::milliseconds(100));
        string tries_str = to_string(tries);
        add_debug_message("try: " + tries_str);
        tries += 1;
        
        client = create_mqtt_client(ADDRESS);
        connect_mqtt_client(&client);
    }

    MQTTClient_setCallbacks(client, NULL, connlost, msgarrvd, delivered);
    
    publish_server_online(&client);
    while(1){
        MQTTClient_yield();
        this_thread::sleep_for(chrono::milliseconds(1000));
        if(!isClientConnected){
            add_debug_message("Client is not connected, trying to reconnect");
            connect_mqtt_client(&client);
        }
    }
}