#include <iostream>

#include <flatbuffers/flatbuffers.h>

#include "client.hpp"
#include "ClientMessage_generated.h"
#include "ServerMessages_generated.h"

namespace FTP {

Client::Client() : Socket(Socket::Type::Client) {}

void Client::Transfer(const std::string & filepath) {
	this->filepath = filepath;

	Status request_status = RequestTransfer();

	if (request_status != Status::Ok) {
		return;
	}

	// It's now safe to begin tranferring the file. In this step we need to send the file's
	// chunks over the network.
}

Client::Status Client::RequestTransfer() {
	// TODO: We need more descriptive status codes here...
	if (SendTransferRequest() != Status::Ok) {
		std::cerr << "Failed to send transfer connection request\n";
		return Status::Error;
	}

	if (ReceiveTransferReply() != Status::Ok) {
		return Status::Error;
	}

	return Status::Ok;
}

Client::Status Client::SendTransferRequest() {
	using namespace Messages::Client;

	// Gather some meta data on the file.

	Status status = Status::Ok;

	flatbuffers::FlatBufferBuilder builder;

	// TODO: Remove hardcoded metadata.
	auto connection_request = CreateConnectionRequest(builder, builder.CreateString(filepath), 100);

	auto client_message = CreateClientMessage(
		builder,
		builder.CreateString(""),
		Message::Message_ConnectionRequest,
		connection_request.Union()
	);

	builder.Finish(client_message);

	status = Send((void *) builder.GetBufferPointer(),builder.GetSize());

	return status;
}

Client::Status Client::ReceiveTransferReply() {
	zmq::message_t reply;
	Status status = Receive(reply);

	if (status != Status::Ok) {
		return status;
	}

	// Verify the response.
	void * reply_bytes = reply.data();
	size_t reply_length = reply.size();

	auto verifier = flatbuffers::Verifier(static_cast<const uint8_t *>(reply_bytes), reply_length);
	if (Messages::Server::VerifyServerMessageBuffer(verifier) == false) {
		std::cerr << "Failed to verify incoming response\n";
		status = Status::Error;
	} else {
		auto server_reply = Messages::Server::GetServerMessage(reply_bytes);

		if (server_reply->status() == Messages::Server::Status_Duplicate) {
			std::cerr << "Failed to obtain transfer, duplicate session\n";
			status = Status::Error;
		} else {
			Messages::Server::Message message_type = server_reply->message_type();

			if (server_reply->message_type() != Messages::Server::Message_ConnectionReply) {
				std::cerr << "Received message other than what was excepted\n";
				status = Status::Error;
			} else {
				auto connection_reply = static_cast<const Messages::Server::ConnectionReply *>(server_reply->message());
				std::string token(connection_reply->client_token()->c_str());
				this->token = std::move(token);
			}
		}
	}

	return status;
}

}