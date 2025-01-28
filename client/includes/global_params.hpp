#pragma once

#include <string>
#include <iostream>
#include <vector>
#include <fstream>
#include <mutex>
#include <queue>

extern std::vector<std::string> server_list;
extern std::ofstream logfile;

extern std::mutex server_list_mutex;

extern std::mutex chat_que_mutex;
extern std::queue<std::string> chat_que;

extern std::mutex sending_msg_mutex;
extern std::queue<std::string> sending_msg_que;