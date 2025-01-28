#include "global_params.hpp"
#include <iostream>
#include <string>

std::vector<std::string> server_list;
std::mutex server_list_mutex;

std::mutex chat_que_mutex;
std::queue<std::string> chat_que;

std::mutex sending_msg_mutex;
std::queue<std::string> sending_msg_que;

std::ofstream logfile("client/src/debug.log", std::ios::app);
