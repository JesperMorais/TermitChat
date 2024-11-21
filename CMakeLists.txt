cmake_minimum_required(VERSION 3.15)
set(CMAKE_TOOLCHAIN_FILE "D:/dev/vcpkg/scripts/buildsystems/vcpkg.cmake" CACHE STRING "")
project(TermitChat)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(TermitChat src/main.cpp)

set(ftxui_DIR "D:/dev/vcpkg/installed/x64-windows/share/ftxui")
find_package(ftxui CONFIG REQUIRED)

target_link_libraries(TermitChat PRIVATE ftxui::screen ftxui::dom ftxui::component)