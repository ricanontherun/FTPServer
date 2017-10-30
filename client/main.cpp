#include <iostream>
#include <fstream>
#include <fcntl.h>
#include <unistd.h>
#include <iomanip>
#include <cmath>

#include <flatbuffers/flatbuffers.h>

#include "streams.hpp"
#include "client.hpp"
#include "ClientMessage_generated.h"
#include "md5_checksum.hpp"

// void transfer_file() {
//     if (argc == 1) {
//         return EXIT_FAILURE;
//     }

//     // Open the file.
//     int file_descriptor;

//     if ( (file_descriptor = open(argv[1], O_RDONLY)) == -1 ) {
//         perror("open");
//         return EXIT_FAILURE;
//     }

//     // Position the internal pointer to front of the file.
//     if (lseek(file_descriptor, 0, SEEK_SET) == -1) {
//         perror("");
//         return EXIT_FAILURE;
//     }

//     MD5_Checksum checksum;
//     zmq::context_t context(1);
//     zmq::socket_t socket(context, ZMQ_REQ);

//     socket.connect("tcp://localhost:55051");

//     zmq::message_t request_message, reply_message;

//     char buffer[BUFSIZ + 1]; // Keep room for null terminator.
//     ssize_t bytes_read;

//     // Read the file in chunks, sending each chunk to the server for storage.
//     while ((bytes_read = read(file_descriptor, buffer, BUFSIZ)) > 0) {
//         checksum.Add(buffer, bytes_read);

//         buffer[bytes_read] = '\0';

//         request_message.rebuild((void *) buffer, static_cast<size_t>(bytes_read));
//         socket.send(request_message);

//         socket.recv(&reply_message);
//     }

//     // Check for a read error. What action can we take here?
//     // Can we reset the file pointer and try again?
//     if ( bytes_read == -1 ) {
//         perror("Read file");
//         return EXIT_FAILURE;
//     }

//     const std::string & checksum_value = checksum.Finish();
//     std::cout << checksum_value << "\n";


//     zmq::message_t finished_message(4);
//     memcpy(finished_message.data(), "DONE", 4);
//     socket.send(finished_message);

//     socket.recv(&reply_message);

//     std::cout << "Read last message from server\n";

//     return EXIT_SUCCESS;
// }

/**
 * Check that a file 1) exists and 2) is readable.
 *
 * @param
 * @return
 */
bool CheckFileAccess(const char * filepath) {
    return access(filepath, R_OK) == 0;
}

int main(int argc, char ** argv) {
    if (argc == 1) {
        std::cerr << "Please provide a filepath\n";
        return EXIT_FAILURE;
    }

    const char * filepath = argv[1];

    if (!CheckFileAccess(filepath)) {
        perror(filepath);
        return EXIT_FAILURE;
    }

    std::ifstream input_file(filepath);

    FTP::Utils::Streams::ChunkRead(input_file, 10, [](const std::string & buffer) {
        std::cout << buffer;
    });

    return 1;

    FTP::Client client;
    FTP::Client::Status status = client.Connect("localhost:55051");

    if (status != FTP::Client::Status::Ok) {
        std::cerr << "Failed to connect to server\n";
        return EXIT_FAILURE;
    }

    client.Transfer(std::string(filepath));
}