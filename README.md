# Project info
This project aims to make a fully terminal driven chat written in c++ using ftxui

# FOR DEVS - HOW TO BUILD PROJECT

## CMakeLists.txt

```
cmake_minimum_required(VERSION 3.15)

project(TermitChat)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(TermitChat src/main.cpp)

# Include the vcpkg toolchain (optional to pass externally)
set(CMAKE_TOOLCHAIN_FILE "D:/dev/vcpkg/scripts/buildsystems/vcpkg.cmake" CACHE STRING "") 

find_package(asio CONFIG REQUIRED)
find_package(ftxui CONFIG REQUIRED)

target_link_libraries(TermitChat PRIVATE ftxui::screen ftxui::dom ftxui::component asio)

# Define _WIN32_WINNT for Windows 10/11 FOR ASIO
target_compile_definitions(TermitChat PRIVATE _WIN32_WINNT=0x0A00)

```

To build Build map: Root/ *"cmake -B build -S . -DCMAKE_TOOLCHAIN_FILE=D:/dev/vcpkg/scripts/buildsystems/vcpkg.cmake"*   *** CHANGE TO YOUR PATH

To compile main.cpp: Root/ *"cmake --build build"*

to build do ./build/debug/termitchat
