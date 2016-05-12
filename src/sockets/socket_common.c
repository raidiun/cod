//socket_common.c

#include <string.h>

#include <sys/socket.h>

#include <arpa/inet.h>

#include <netdb.h> // netdb: functions for hostname lookup

#include "../debug.h"

#include "socket.h"

SockFD_t createSocket() {
	int sockfd = socket(AF_INET, SOCK_DGRAM, 0);
	if (sockfd < 0) { ERROREXIT("createSocket: Error creating socket",1); }
	DEBUGMSG("Socket fd created.\r\n");
	
	return (SockFD_t) sockfd;
	}

SockAddr_t getAddressWithHost(char const * const host, int port, int ipv6) {
	struct hostent* server = gethostbyname2(host,AF_INET); // Lookup IPv4 address
	if (server == NULL) { ERROREXIT("getAddress: Error finding host",2); }
	DEBUGMSG("Got server address: %u.%u.%u.%u\r\n",(unsigned char)server->h_addr[0],(unsigned char)server->h_addr[1],(unsigned char)server->h_addr[2],(unsigned char)server->h_addr[3]);
	
	struct sockaddr_in serverAddr;
	serverAddr.sin_family = AF_INET;        // Set family to internet
	serverAddr.sin_port = htons(port);  // Set port in network order
	memcpy( (void*) &(serverAddr.sin_addr.s_addr), server->h_addr, server->h_length); // Copy address
	memset(&(serverAddr.sin_zero), '\0', 8); // Zero struct
	
	return (SockAddr_t) serverAddr;
	}

SockAddr_t getAddressWithIP(char const * const addr, int port, int ipv6) {
	struct in_addr ipAddress;
	int err = inet_aton(addr, &ipAddress);
	if ( err == 0 ) { ERROREXIT("getAddress: Error parsing IP address",2); }
	
	struct sockaddr_in serverAddr;
	serverAddr.sin_family = AF_INET;        // Set family to internet
	serverAddr.sin_port = htons(port);  // Set port in network order
	memcpy( (void*) &(serverAddr.sin_addr), &ipAddress, sizeof(struct in_addr)); // Copy address
	memset(&(serverAddr.sin_zero), '\0', 8); // Zero struct
	
	return (SockAddr_t) serverAddr;
	}
