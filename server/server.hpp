#ifndef FTP_SERVER_H
#define FTP_SERVER_H

#include <map>

#include "socket/socket.hpp"

namespace FTP {

namespace Messages { namespace Client {
	class ClientMessage;
	class ConnectionRequest;
}}

void hash(unsigned char * data, size_t length);

class Server : public Socket {
public:
	struct Transfer {
		std::string filepath;
	};

	Server();
	void Start();

private:
	// Maps client tokens to transfer metadata;
	std::map<std::string, Transfer> transfers;

	bool ValidateClientRequest(const zmq::message_t & request);

	Status HandleClientRequest(const zmq::message_t & request, zmq::message_t & reply);

	void HandleConnectionRequest(const Messages::Client::ConnectionRequest * request, zmq::message_t & reply);
	void GenerateClientToken(std::string & token);
};

}

#endif