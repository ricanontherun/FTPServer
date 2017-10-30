cmake_minimum_required(VERSION 3.5.1)
project(ftp-client)

SET(
        SOURCES
        main.cpp
        client.cpp
        ../utils/socket/socket.cpp
)

include_directories(
        ../vendor
        ../vendor/openssl/include
        ../vendor/flatbuffers/include
        ../utils
        ../proto/include
)

add_executable(ftp-client ${SOURCES})

FIND_LIBRARY(
        ZMQ_LIB
        NAMES zmq libzmq
)

TARGET_LINK_LIBRARIES(ftp-client ${ZMQ_LIB})

FIND_LIBRARY(
        OPENSSL
        NAMES openssl crypto
)

TARGET_LINK_LIBRARIES(ftp-client ${OPENSSL})