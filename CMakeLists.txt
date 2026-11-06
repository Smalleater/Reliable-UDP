cmake_minimum_required(VERSION 3.23)

project(Reliable-UDP)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

#[[This is for the client]]
add_executable(RUDP-Client)

target_sources(RUDP-Client
    PRIVATE
        src/client.cpp
        src/udpSocket.cpp
)

target_include_directories(RUDP-Client
    PRIVATE
        ${PROJECT_SOURCE_DIR}/include
)

if (WIN32)
    target_link_libraries(RUDP-Client
        PRIVATE
            ws2_32
    )
endif()

#[[This is for the server]]
add_executable(RUDP-Server)

target_sources(RUDP-Server
    PRIVATE
        src/server.cpp
        src/udpSocket.cpp
)

target_include_directories(RUDP-Server
    PRIVATE
        ${PROJECT_SOURCE_DIR}/include
)

if (WIN32)
    target_link_libraries(RUDP-Server
        PRIVATE
            ws2_32
    )
endif()