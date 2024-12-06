#pragma once

#include <ftxui/component/component.hpp>
#include <ftxui/dom/elements.hpp>
#include <ftxui/component/screen_interactive.hpp>
#include <iostream>

using namespace std;
using namespace ftxui;

//Håller koll på vilken skärm ska visas
enum class AppState {
    EnterClientName,
    ServerOW,
    ServerDetails,
    InChat,
    Exit
};


/// @brief MAIN FUNCTION TO RUN FTXUI during seassion
int rund_app();