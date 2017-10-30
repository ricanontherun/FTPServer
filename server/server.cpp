#include <iostream>
#include <algorithm>
#include <chrono>

#include <openssl/sha.h>
#include <flatbuffers/flatbuffers.h>

#include "server.hpp"
#include "ClientMessage_generated.h"
#include "ServerMessages_generated.h"

namespace FTP {

Server::Server() : Socket(Socket::Type::Server) {}

void Server::Start() {
	Status receive_status, send_status;
	zmq::message_t request, reply;

	while (true) {
		if ( Receive(request) == Status::Error ) {
			std::cerr << "Failed to receive client message\n";
			continue;
		}

		if (!ValidateClientRequest(request)) {
			continue;
		}

		HandleClientRequest(request, reply);
	}
}

bool Server::ValidateClientRequest(const zmq::message_t & request) {
	using namespace Messages::Client;

	auto verifier = flatbuffers::Verifier(static_cast<const uint8_t *>(request.data()), request.size());

	return VerifyClientMessageBuffer(verifier);
}

Server::Status Server::HandleClientRequest(const zmq::message_t & request, zmq::message_t & reply) {
	using namespace Messages::Client;

	// Decode the client message.
	auto client_message = GetClientMessage(request.data());

	// Make a decision from the message type.
	switch (client_message->message_type()) {
		case Message::Message_ConnectionRequest:
			HandleConnectionRequest(static_cast<const ConnectionRequest *>(client_message->message()), reply);
			break;
		case Message::Message_NONE:
		default:
			std::cerr << "Unknown message type, ignoring\n";
			return Status::Error;
	}

	return Status::Ok;
}

void Server::HandleConnectionRequest(const Messages::Client::ConnectionRequest * request, zmq::message_t & reply) {
	using namespace Messages::Server;

	std::string filepath = request->filepath()->str();

	// Sanitize the requested filepath by erasing all leading non alphanumeric characters.
	auto filepath_begin = std::begin(filepath);
	auto filepath_end = std::end(filepath);
	filepath.erase(filepath_begin, std::find_if(filepath_begin, filepath_end, ::isalnum));

	// Lookup the requested filepath, checking if a transfer is already in progress.
	auto transfer_it = std::find_if(std::begin(transfers), std::end(transfers), [&filepath](auto & transfer_kv) {
		return transfer_kv.second.filepath == filepath;
	});

	flatbuffers::FlatBufferBuilder builder;

	if (transfer_it != std::end(transfers)) {
		std::cout << "Duplicate file transfer\n";
		auto server_reply = CreateServerMessage(builder, Status_Duplicate);
		builder.Finish(server_reply);
	} else {
		std::cout << "Creating transfer record for " << filepath << "\n";

		std::string token;
		GenerateClientToken(token);
		transfers[token] = Transfer{.filepath = filepath};

		auto connection_reply = CreateConnectionReply(builder,builder.CreateString(token));

		auto server_reply = CreateServerMessage(
			builder,
			Status_Ok,
			Messages::Server::Message::Message_ConnectionReply,
			connection_reply.Union()
		);

		builder.Finish(server_reply);
	}

	// Is this a good way to handle sends? Does every HandleX method take care of this?
	Send((void *) builder.GetBufferPointer(), builder.GetSize());
}

void Server::GenerateClientToken(std::string & token) {
	using clock = std::chrono::high_resolution_clock;

	std::string now = std::to_string(clock::to_time_t(clock::now()));

	size_t sha256_string_length = SHA256_DIGEST_LENGTH * 2 + 1;

	unsigned char digest[SHA256_DIGEST_LENGTH];
	SHA256((unsigned char *)now.c_str(), now.length(), (unsigned char *) &digest);

	char sha256_string[sha256_string_length];

	for (int i = 0; i < SHA256_DIGEST_LENGTH; ++i) {
	    sprintf(&sha256_string[i*2], "%02x", (unsigned int) digest[i]);
	}

	sha256_string[strlen(sha256_string) - 1] = '\0';

	token.assign(sha256_string);
}

}