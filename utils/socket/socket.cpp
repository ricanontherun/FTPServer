#include "socket.hpp"

Socket::Socket(Type type) :
	context(zmq::context_t(1)),
	socket(zmq::socket_t(context, static_cast<int>(type))) {}

Socket::Status Socket::Connect(const std::string & address) {
	Status status = Status::Ok;

	try {
		socket.connect("tcp://" + address);
	} catch (zmq::error_t & error) {
		status = Status::Error;
	}

	return status;
}

Socket::Status Socket::Bind(const std::string & address) {
	Status status = Status::Ok;

	try {
		socket.bind("tcp://" + address);
	} catch (zmq::error_t & error) {
		status = Status::Error;
	}

	return status;
}

Socket::Status Socket::Send(void * buffer, size_t length) {
	Status status = Status::Ok;

	try {
		zmq::message_t message(buffer, length);

		if ( !socket.send(message) ) {
			status = Status::Error;
		}
	} catch (zmq::error_t & error) {
		status = Status::Error;
	}

	return status;
}

Socket::Status Socket::Receive(zmq::message_t &request) {
	Status status = Status::Ok;

	try {
		if (!socket.recv(&request)) {
			status = Status::Error;
		}
	} catch (zmq::error_t & error) {
		status = Status::Error;
	}

	return status;
}