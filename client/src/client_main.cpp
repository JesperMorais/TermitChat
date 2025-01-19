#include <iostream>
#include <thread>
#include <chrono>
#include <ftxui/component/component.hpp>
#include <ftxui/dom/elements.hpp>
#include <ftxui/component/screen_interactive.hpp>

#include <MQTTClient.h>
#include <MQTTAsync.h>
#include "handle_uuid.hpp"
#include "menus/menu_state.hpp"
#include "handle_mqtt.hpp"
#include "global_params.hpp"

using namespace std;
using namespace ftxui;


int main() {
    //CLEAR THE LOG FILE
    std::ofstream("client/src/debug.log", std::ios::trunc);
    
    thread_params tp;
    if (getenv("IN_NEW_TERMINAL") == nullptr) {
        std::string command = "start cmd.exe /k \"set IN_NEW_TERMINAL=1 && build\\client\\debug\\client\"";
        std::system(command.c_str());
        return 0;
    }
    
    thread ftxui_task(rund_app, &tp);
    thread mqtt_task(mqtt_task, &tp);
    ftxui_task.detach();

    //MQTT kör en loop så vi kommer inte förbi denna raden, därför kör vi en egen tråd för ftxui
    mqtt_task.join();

    return 0;
}