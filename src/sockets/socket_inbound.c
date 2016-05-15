//socket_inbound.c

#include <sys/socket.h>

#include "socket.h"

int bindSocket(SockFD_t sockfd, SockAddr_t listenAddr) {
	// Sneaky not having to wait for timeout stuff...
	int optval = 1;
	setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, (const void *)&optval , sizeof(int));
	
	int err = bind((int) sockfd, (struct sockaddr*) &listenAddr, sizeof(listenAddr));
	if(err < 0) { DEBUGMSG("bindSocket: Error binding socket"); return DHTD_ERROR; }
	return 1;
	}
