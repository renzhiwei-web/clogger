cmake_minimum_required (VERSION 3.2)

project (logger)

set(CMAKE_CXX_STANDARD 17)

include_directories(./utility)

add_executable(main main.cpp ./utility/Logger.cpp)
