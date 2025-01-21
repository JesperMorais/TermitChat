#include "global_params.hpp"
#include <iostream>
#include <string>

std::vector<std::string> server_list;
std::mutex server_list_mutex;

std::ofstream logfile("client/src/debug.log", std::ios::app);