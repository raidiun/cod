//socket_outbound.c

#include <sys/socket.h>

#include "../debug.h"

#include "socket.h"

int connectSocket(SockFD_t sockfd, SockAddr_t serverAddr) {
	int err = connect(sockfd, (struct sockaddr *) &serverAddr, sizeof(struct sockaddr));
	if (err < 0) { ERROREXIT("connectSocket: Error connecting to socket",3); }
	DEBUGMSG("Socket connected.\r\n");
	// Possibly make failing less harsh than exiting the program...
	return 0;
	}
