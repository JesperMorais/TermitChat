#pragma once

#include <ftxui/component/component.hpp>
#include <ftxui/dom/elements.hpp>
#include <ftxui/component/screen_interactive.hpp>
#include <iostream>
#include <functional>
#include <string>
#include <vector>

using namespace std;
using namespace ftxui;

/// @brief Funktion som kör menu som tar emot användarens nman
/// @param on_continue funktions pekare till funktion som tar användaren vidare i programmet
/// @return Component, dvs hela skärmen som ska renderas
ftxui::Component MakeEnterNameMenu(std::string* input_content_client_name, std::string* client_name, std::function<void()> on_submit);

// Denna funktion returnerar ett Component för att visa en lista av servrar.
// on_connect anropas när användaren trycker på knappen för att ansluta.
ftxui::Component MakeServerOverview(std::function<void(const std::string&)> on_connect);

ftxui::Component MakeServerDetails(const std::string* serverName ,std::function<void(const std::string&)> on_connect);