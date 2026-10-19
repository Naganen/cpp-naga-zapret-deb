cmake_minimum_required(VERSION 3.16)
project(naga-zapret VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_AUTOMOC ON)

find_package(Qt6 REQUIRED COMPONENTS Widgets Network)

add_executable(naga-zapret main.cpp)
target_link_libraries(naga-zapret PRIVATE Qt6::Widgets Qt6::Network)