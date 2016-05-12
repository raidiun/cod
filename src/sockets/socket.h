// sockets.h

#ifndef DHTD_SOCKETS_H
#define DHTD_SOCKETS_H

#include <netinet/in.h>

typedef int
	SockFD_t;

typedef struct sockaddr_in
	SockAddr_t;

// socket_common

SockFD_t createSocket();

SockAddr_t getAddressWithHost(char const * const host, int port, int ipv6);

SockAddr_t getAddressWithIP(char const * const addr, int port, int ipv6);

// socket_outbound

int connectSocket(SockFD_t sockfd, SockAddr_t serverAddr);

// socket_inbound

int bindSocket(SockFD_t sockfd, SockAddr_t listenAddr);

#endif//DHTD_SOCKETS_H
