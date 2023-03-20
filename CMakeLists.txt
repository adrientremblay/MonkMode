cmake_minimum_required(VERSION 3.16)
project(MonkMode)

set(CMAKE_CXX_STANDARD 14)

find_package(Curses REQUIRED)
include_directories(${CURSES_INCLUDE_DIR})

find_package(Threads REQUIRED)

find_package(Boost REQUIRED COMPONENTS serialization)
include_directories(${Boost_INCLUDE_DIRS})

add_executable(MonkMode
        main.cpp)

target_link_libraries(MonkMode ${CURSES_LIBRARIES} Threads::Threads  ${Boost_LIBRARIES})