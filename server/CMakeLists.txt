add_executable(server src/server_main.cpp)

find_package(asio CONFIG REQUIRED)
find_package(ftxui CONFIG REQUIRED)

target_link_libraries(server PRIVATE asio ftxui::screen ftxui::dom ftxui::component)

add_compile_definitions(_WIN32_WINNT=0x0A00)