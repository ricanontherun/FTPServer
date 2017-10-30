#ifndef FTP_SOCKET_H
#define FTP_SOCKET_H

#include <cppzmq/zmq.hpp>

class Socket {
public:
	using message_buffer_t = void *;
	using message_length_t = size_t;
	
	enum class Type {
		Server = ZMQ_REP,
		Client = ZMQ_REQ
	};

	enum class Status {
		Ok = 0,
		Error
	};

	Socket(Type type);
	Status Connect(const std::string & address);
	Status Bind(const std::string & address);	
	Status Send(void * buffer, size_t length);

	// TODO: Remove explicit referece to zmq from public interface.
	Status Receive(zmq::message_t &request);
protected:
	zmq::context_t context;
	zmq::socket_t socket;

};

#endif