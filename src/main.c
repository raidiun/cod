#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "sockets/socket.h"

#define PORT 9898
#define BUFSIZE 1024

int main(int argc, char** argv) {

	int port;
	if( argc == 2 ) {
		port = atoi(argv[1]);
		}
	else {
		port = PORT;
		}

	SockFD_t sockfd = createSocket();

	SockAddr_t listenOn = getAddressWithIP("127.0.0.1",port,0);

	int err = bindSocket(sockfd, listenOn);
	if(err != 1) {
		printf("Bind error: %i\r\n",err);
		exit(1);
		}
	else {
		printf("Bound to port %i\r\n",port);
		}

	int* buf = malloc(sizeof(int)*BUFSIZE);
	while(1) {
		memset(buf,0,sizeof(int)*BUFSIZE);
		err = recvfrom(sockfd, buf, BUFSIZE, 0, (struct sockaddr*) NULL, 0);
		if (err < 0) {
			printf("recvfrom error: %i\r\n",err);
			}

		printf("server received %d/%d bytes: %s\r\n", (int) strlen((char*) buf), err, (char*) buf);
		}
	}
