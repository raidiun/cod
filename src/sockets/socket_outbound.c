//socket_outbound.c

#include <sys/socket.h>

#include "socket.h"

int connectSocket(SockFD_t sockfd, SockAddr_t serverAddr) {
	int err = connect(sockfd, (struct sockaddr *) &serverAddr, sizeof(struct sockaddr));
	if (err < 0) { DEBUGMSG("connectSocket: Error connecting to socket"); return DHTD_ERROR; }
	return 1;
	}
