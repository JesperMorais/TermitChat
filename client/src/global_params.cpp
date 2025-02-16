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

//Enum for the different states of the application
enum clientColorChoices{
    BLUE,
    RED,
    WHITE
};

//Color list for clients
const std::vector<ftxui::Color> clientColorList = {ftxui::Color::Blue, ftxui::Color::Red, ftxui::Color::White};
