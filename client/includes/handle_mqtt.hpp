#include <iostream>
#include <string>

void mqtt_task(void* thread_params);

void mqtt_connect();

void subscribe_to_topic();

int msgarrvd(void *context, char *topicName, int topicLen, MQTTClient_message *message);

void connlost(void *context, char *cause);

void delivered(void *context, MQTTClient_deliveryToken dt);
