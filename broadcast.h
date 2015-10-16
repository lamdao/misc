#ifndef __BROADCASTER_H
#define __BROADCASTER_H
//--------------------------------------------------------------------------
#include <sys/socket.h>
#include <arpa/inet.h>
//--------------------------------------------------------------------------
class Broadcast
{
private:
	struct sockaddr_in addr;	// Broadcast Address
	int sock;					// Socket
public:
	Broadcast(const char *ip, int port, bool sender = false);
	~Broadcast();

	int Send(void *buffer, int size);
	int Receive(void *buffer, int size);
	int Handle() { return sock; }
};
//--------------------------------------------------------------------------
#endif
