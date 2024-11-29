#include <iostream>
#include "global_params.hpp"

//string of input of user
string input_content;

/// @brief string of incoming data
string recv_data;

/// @brief checks if the client is connected
bool isClientConnected = false;

/// @brief Mutex for dataque
mutex data_mutex;

/// @brief queue to hold inputed messages, 
queue<string> message_queue;

/// @brief list of debug messages to display on screen
vector<string> debug_messages;

/// @brief mutex for debug messages and queue
mutex debug_mutex;
