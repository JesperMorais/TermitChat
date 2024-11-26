# Add the executable
add_executable(client src/client_main.cpp)

# Include Asio headers
find_package(asio CONFIG REQUIRED) # Works if vcpkg toolchain is active
find_package(ftxui CONFIG REQUIRED)

target_link_libraries(client PRIVATE asio ftxui::screen ftxui::dom ftxui::component)

add_compile_definitions(_WIN32_WINNT=0x0A00)