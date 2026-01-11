 cmake_minimum_required(VERSION 3.31)
project(SUB3
        VERSION 0.7
        DESCRIPTION "Telegram userbot"
        LANGUAGES C CXX
)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_SCAN_FOR_MODULES ON)

set(PREFIX "${CMAKE_SOURCE_DIR}")

include("CMake/Tasks/AddDependencies.cmake") # Add all subdirs (third_party/)

include("CMake/Targets/SUB3.cmake") # Add SUB3 executable
