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

void disconnect_cb(void *context, MQTTProperties *properties, MQTTReasonCodes reasonCode){
    add_debug_message("Client was disconnected!");
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
        isClientConnected = true;
        MQTTClient_setDisconnected(*client, NULL, disconnect_cb);
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

    string msg = (string)message;
    add_debug_message("Published message to topic: " + topic + " with message: " + msg);
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
    
    publish_server_online(&client);
    while(1){
        MQTTClient_yield();
        this_thread::sleep_for(chrono::milliseconds(1000));
    }
}