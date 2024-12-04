# Lägg till exekverbara filer
add_executable(server
    src/server_main.cpp
    src/debug.cpp
    src/asio_task.cpp
    src/global_params.cpp
    src/mqtt_client.cpp
    # Lägg till andra källfiler här om det behövs
)

# Definiera Windows-version
add_definitions(-D_WIN32_WINNT=0x0A00)

# Länka FTXUI-biblioteken
target_link_libraries(server PRIVATE
    ftxui::screen
    ftxui::dom
    ftxui::component eclipse-paho-mqtt-c::paho-mqtt3a 
    eclipse-paho-mqtt-c::paho-mqtt3c 
    eclipse-paho-mqtt-c::paho-mqtt3as 
    eclipse-paho-mqtt-c::paho-mqtt3cs
    Boost::uuid
)

# Om du har egna inkluderingsfiler i 'includes'-katalogen
target_include_directories(server PRIVATE
    "${CMAKE_CURRENT_SOURCE_DIR}/includes"
)
