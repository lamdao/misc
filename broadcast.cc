#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
//--------------------------------------------------------------------------
#include "broadcast.h"
//--------------------------------------------------------------------------
static void ShowError(const char *msg)
{
	fprintf(stderr, "%s: %s\n", msg, strerror(errno));
	exit(-1);
}
//--------------------------------------------------------------------------
Broadcast::Broadcast(const char *ip, int port, bool sender)
{
	// Create socket for sending/receiving datagrams
	if ((sock = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP)) < 0)
		ShowError("socket() failed");

	// Construct local address structure
	memset(&addr, 0, sizeof(addr));
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = sender ? inet_addr(ip) : htonl(INADDR_ANY);
	addr.sin_port = htons(port);

	if (sender) {
		// Set socket to allow broadcast
		int e = 1;
		if (setsockopt(sock, SOL_SOCKET, SO_BROADCAST, &e, sizeof(e)) < 0)
			ShowError("setsockopt() failed");
	} else {
		// Bind to the broadcast port
		if (bind(sock, (struct sockaddr *) &addr, sizeof(addr)) < 0)
			ShowError("bind() failed");
	}
}
//--------------------------------------------------------------------------
Broadcast::~Broadcast()
{
	close(sock);
}
//--------------------------------------------------------------------------
int Broadcast::Send(void *buffer, int size)
{
	return sendto(sock,buffer,size,0,(struct sockaddr *)&addr,sizeof(addr));
}
//--------------------------------------------------------------------------
int Broadcast::Receive(void *buffer, int size)
{
	return recvfrom(sock,buffer,size,0,NULL,0);
}
//--------------------------------------------------------------------------
#ifdef TEST_BROADCAST
//--------------------------------------------------------------------------
int client(char *ip, int port)
{
	char buf[256];
	Broadcast bc(ip, port);
	int n = bc.Receive(buf, sizeof(buf)-1);
	buf[n] = 0;
	printf("Received: %s\n", buf);

	return 0;
}
//--------------------------------------------------------------------------
int server(char *ip, int port, char *msg)
{
	int n = strlen(msg);
	Broadcast bc(ip, port, true);
	while (1) {
		bc.Send(msg, n);
		sleep(3);
	}
	return 0;
}
//--------------------------------------------------------------------------
int main(int argc, char *argv[])
{
	if (argc < 3) {
		printf("Usage:\n\t%s <ip> <port> [msg]\n");
		return 0;
	}
	char *ip = argv[1];
	int port = atoi(argv[2]), e;

	if (argv[3] != NULL)
		e = server(ip, port, argv[3]);
	else
		e = client(ip, port);

	return e;
}
//--------------------------------------------------------------------------
#endif	// TEST_BROADCAST
