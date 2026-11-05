cmake_minimum_required(VERSION 3.23)

project(Reliable-UDP)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_executable(Reliable-UDP)

target_sources(Reliable-UDP
    PRIVATE
        src/main.cpp
        src/udpSocket.cpp
)

target_include_directories(Reliable-UDP
    PRIVATE
        ${PROJECT_SOURCE_DIR}/include
)