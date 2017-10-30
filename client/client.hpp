#ifndef FTP_CLIENT_H
#define FTP_CLIENT_H

#include "socket/socket.hpp"

namespace FTP {
	class Client : public Socket {
	public:
		struct Transfer {
			int fd;
		};

		Client();
		void Transfer(const std::string & filepath);
	private:
		std::string token;
		std::string filepath;
		bool ok;

		// Request the transfer of a file to the server. Successful transfer
		// requests yield a token which the client uses for all subsequent requests
		// to the server.
		Status RequestTransfer();

		// Construct and send a transfer request message.
		Status SendTransferRequest();

		// Block and wait for decode the server's reply.
		Status ReceiveTransferReply();
	};

}

#endif