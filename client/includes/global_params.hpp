#pragma once

#include <string>
#include <iostream>
#include <vector>
#include <fstream>
#include <mutex>

extern std::vector<std::string> server_list;
extern std::ofstream logfile;

extern std::mutex server_list_mutex;
