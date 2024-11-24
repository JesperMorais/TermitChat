# Project info
This project aims to make a fully terminal driven chat written in c++.
Using ftxui for Terminal UX and Asio for network.

## FOR DEVS - HOW TO BUILD PROJECT

# ADD CMakeLists.txt in:
```client/```

```server/```



Change the CMakeList.txt to the one below.
**NOTE** CHANGE the PATHS to YOUR vcpkg dir.

##  ROOT CMakeLists.txt

```
cmake_minimum_required(VERSION 3.15)

# Set the vcpkg toolchain file BEFORE project()
set(CMAKE_TOOLCHAIN_FILE "D:/dev/vcpkg/scripts/buildsystems/vcpkg.cmake" CACHE STRING "")

# Define the project
project(TermitChat)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Add subdirectories for client and server
add_subdirectory(client)
add_subdirectory(server)

# Global dependencies (optional: could move to specific CMakeLists.txt files)
find_package(asio CONFIG REQUIRED)
find_package(ftxui CONFIG REQUIRED)

# Define _WIN32_WINNT globally for Asio
add_compile_definitions(_WIN32_WINNT=0x0A00)
```

## Server / Client CMakeLists.txt
**CHANGE PATHS AS NEEDED**

**THIS IS A CLIENT EXAMPLE**
```
# Add the executable
add_executable(client src/client_main.cpp)

# Include Asio headers
find_package(asio CONFIG REQUIRED) # Works if vcpkg toolchain is active
target_link_libraries(client PRIVATE asio)
```

To build Build map: Root/ *"cmake -B build -S . -DCMAKE_TOOLCHAIN_FILE=D:/dev/vcpkg/scripts/buildsystems/vcpkg.cmake"*   *** CHANGE TO YOUR PATH

To compile The whole project Root/ *"cmake --build build"*

to run specific part: 

Server: ./build/server/debug/server

Client: ./build/client/debug/client

TODO: ADD BETTER/EASIER WAY TO RUN EACH

### DEBUG / BUILD ISSUES

1. Normal Issues can be fixed with removal of BUILD map and rebuild project.
