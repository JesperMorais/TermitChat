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




