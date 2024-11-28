cmake_minimum_required(VERSION 3.21)


# Om CMAKE_TOOLCHAIN_FILE inte är satt, försök använda VCPKG_ROOT
if(NOT DEFINED CMAKE_TOOLCHAIN_FILE)
    if(DEFINED ENV{VCPKG_ROOT})
        set(CMAKE_TOOLCHAIN_FILE "$ENV{VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake" CACHE STRING "")
    else()
        message(FATAL_ERROR "CMAKE_TOOLCHAIN_FILE is not set and VCPKG_ROOT environment variable is not defined.")
    endif()
endif()

project(TermitChat)

include_directories("${CMAKE_SOURCE_DIR}/vcpkg_installed/x64-windows/include")

# Använd C++17
set(CMAKE_CXX_STANDARD 17)

find_package(ftxui CONFIG REQUIRED)
find_package(asio CONFIG REQUIRED)

# Inkludera underkatalogerna
add_subdirectory(client)
add_subdirectory(server)
