#pragma once

#include <ftxui/component/component.hpp>
#include <ftxui/dom/elements.hpp>
#include <ftxui/component/screen_interactive.hpp>
#include <iostream>

using namespace std;
using namespace ftxui;

typedef struct{
    std::mutex m;
    std::condition_variable cv; // för att vänta på namn av användaren
    std::condition_variable mqtt_message_cv; // för att vänta på mqtt meddelanden
    std::string clientInput_username;
    std::string mqttClient_ID;
    std::string current_server_name;
    bool ready = false;
    bool processed = false;
    bool usernameSet = false;
}thread_params;

//Håller koll på vilken skärm ska visas
enum class AppState {
    EnterClientName,
    ServerSelect,
    ServerDetails,
    InChat,
    Exit
};


/// @brief MAIN FUNCTION TO RUN FTXUI during seassion
int rund_app(void* thead_params);