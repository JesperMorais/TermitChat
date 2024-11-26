cmake_minimum_required(VERSION 3.15)

# Read from .env file
file(READ "${CMAKE_SOURCE_DIR}/.env" ENV_FILE)
string(REPLACE ";" "\\;" ENV_FILE "${ENV_FILE}")

# Function to extact varaibles from .env file
function(load_env_variable VAR_NAME)
    string(REGEX MATCH "^${VAR_NAME}=.*$" MATCH "${ENV_FILE}")
    if(MATCH)
        string(REGEX REPLACE "^${VAR_NAME}=(.*)$" "\\1" ${VAR_NAME} "${MATCH}")
        message(STATUS "Loaded ${VAR_NAME} = ${${VAR_NAME}}")
    else()
        message(WARNING "Environment variable ${VAR_NAME} not found in .env file")
    endif()
endfunction()

# Load environment variables
load_env_variable(CMAKE_TOOLCHAIN_FILE)
load_env_variable(FTXUI_DIR)

set(CMAKE_TOOLCHAIN_FILE "${CMAKE_TOOLCHAIN_FILE}" CACHE STRING "")
project(TermitChat)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Add subdirectories for client and server
add_subdirectory(client)
add_subdirectory(server)





