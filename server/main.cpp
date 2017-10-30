#include <fstream>
#include <iostream>
#include <cppzmq/zmq.hpp>
#include <md5_checksum.hpp>
#include <cstring>
#include <ClientMessage_generated.h>

#include "server.hpp"

void extract(const zmq::message_t & message, std::string & buffer) {
    buffer.assign((char *) message.data(), message.size());
}

int main() {
    FTP::Server server;
    FTP::Server::Status status = server.Bind("*:55051");

    if (status != FTP::Server::Status::Ok) {
        perror("Couldn't listen on port");
        return EXIT_FAILURE;
    }

    server.Start();

    return EXIT_SUCCESS;
}