# Lägg till exekverbara filer
add_executable(client
    src/client_main.cpp
    # Lägg till andra källfiler här om det behövs
)

# Definiera Windows-version
add_definitions(-D_WIN32_WINNT=0x0A00)

# Länka FTXUI-biblioteken
target_link_libraries(client PRIVATE
    ftxui::screen
    ftxui::dom
    ftxui::component eclipse-paho-mqtt-c::paho-mqtt3a 
    eclipse-paho-mqtt-c::paho-mqtt3c 
    eclipse-paho-mqtt-c::paho-mqtt3as 
    eclipse-paho-mqtt-c::paho-mqtt3cs
    Boost::uuid
)


# Om du har egna inkluderingsfiler i 'includes'-katalogen
target_include_directories(client PRIVATE
    "${CMAKE_CURRENT_SOURCE_DIR}/includes"
)
