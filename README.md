# CHANGE

To build Build map: Root/ *cmake -B build -S . -DCMAKE_TOOLCHAIN_FILE=D:/dev/vcpkg/scripts/buildsystems/vcpkg.cmake*   *** CHANGE TO YOUR PATH

To compile main.cpp: Root/ *cmake --build build*



## CMakeLists.txt

```
cmake_minimum_required(VERSION 3.15)
set(CMAKE_TOOLCHAIN_FILE "****D:/dev/vcpkg/scripts/buildsystems/vcpkg.cmake****" CACHE STRING "") // CHANGE THISSS TO YOUR PATH
project(TermitChat)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(TermitChat src/main.cpp)

set(ftxui_DIR "****D:/dev/vcpkg/installed/x64-windows/share/ftxui****") //CHANGE TO YOUR PATHH
find_package(ftxui CONFIG REQUIRED)

target_link_libraries(TermitChat PRIVATE ftxui::screen ftxui::dom ftxui::component)
```
