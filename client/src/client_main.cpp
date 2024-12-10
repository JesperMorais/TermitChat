#include <iostream>
#include <thread>
#include <chrono>
#include <ftxui/component/component.hpp>
#include <ftxui/dom/elements.hpp>
#include <ftxui/component/screen_interactive.hpp>

#include <MQTTClient.h>
#include "handle_uuid.hpp"
#include "menus/menu_state.hpp"

using namespace std;
using namespace ftxui;

int main() {
    //UI för att sätta CLIENTNAMN -> leder till att clienten får ett unikt ID
    //UI & MQTT hitta serverar under subscribe - server/announcment
    //UI Kunna välja bland olika server och connecta till dem.

    // Relaunch the program in a new terminal if not already in one
    if (getenv("IN_NEW_TERMINAL") == nullptr) {
        std::string command = "start cmd.exe /k \"set IN_NEW_TERMINAL=1 && build\\client\\debug\\client\"";
        std::system(command.c_str());
        return 0;
    }
    
    thread ftxui_task(rund_app);
    ftxui_task.join();


    return 0;
}
