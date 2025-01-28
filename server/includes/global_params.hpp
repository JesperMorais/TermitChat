#pragma once
#include <iostream>
#include <string>
#include <mutex>
#include <vector>
#include <queue>

using namespace std;

extern string serverName;

extern string input_content_server_name;

//string of input of user
extern string input_content;

/// @brief string of incoming data
extern string recv_data;

/// @brief Mutex for dataque
extern mutex data_mutex;

/// @brief queue to hold inputed messages, 
extern queue<string> message_queue;

/// @brief list of debug messages to display on screen
extern vector<string> debug_messages;

/// @brief mutex for debug messages and queue
extern mutex debug_mutex;

/// @brief hold the servers "unice" ID
extern string serverID;