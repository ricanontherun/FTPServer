cmake_minimum_required(VERSION 3.5.1)
project(ftp-server)

SET(
        SOURCES
        main.cpp
        ../utils/socket/socket.cpp
        server.cpp
)

include_directories(
        ../vendor
        ../utils
        ../vendor/flatbuffers/include
        ../proto/include
)

add_executable(ftp-server ${SOURCES})

FIND_LIBRARY(
        ZMQ_LIB
        NAMES zmq libzmq
)

TARGET_LINK_LIBRARIES(ftp-server ${ZMQ_LIB})

FIND_LIBRARY(
        OPENSSL
        NAMES openssl crypto
)

TARGET_LINK_LIBRARIES(ftp-server ${OPENSSL})

