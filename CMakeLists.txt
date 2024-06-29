cmake_minimum_required (VERSION 3.2)

project (logger)

set(CMAKE_CXX_STANDARD 17)

include_directories(./utility)

set(LOGGER ./utility/Logger.cpp)
add_library(log SHARED ${LOGGER})

add_executable(main main.cpp)

target_link_directories(main PUBLIC ./build)
target_link_libraries(main log)
