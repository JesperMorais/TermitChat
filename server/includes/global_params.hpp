#pragma once
#include <iostream>
#include <string>
#include <mutex>
#include <vector>
#include <queue>

using namespace std;

//string of input of user
extern string input_content;

/// @brief string of incoming data
extern string recv_data;

/// @brief checks if the client is connected
extern bool isClientConnected;

/// @brief Mutex for dataque
extern mutex data_mutex;

/// @brief queue to hold inputed messages, 
extern queue<string> message_queue;

/// @brief list of debug messages to display on screen
extern vector<string> debug_messages;

/// @brief mutex for debug messages and queue
extern mutex debug_mutex;