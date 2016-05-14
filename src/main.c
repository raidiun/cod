#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <unistd.h>
#include <pthread.h>

#include "sockets/socket.h"
#include "messages/message.h"

#include "crypto/crypto.h"
#include "config/config.h"
#include <openssl/rand.h>

#define PORT 9898
#define BUFSIZE 4096

void hexDump(uint8_t* bytes, int len) {
	printf("XD:");
	for(int i=0; i<len; i++) {
		printf("%02x",bytes[i]);
		}
	printf("\r\n");
	}

void printMessage(Message_t message) {
	printf("Message built:\r\n Type:    %i\r\n nodeID:  %016llx\r\n length:  %i\r\n content: %.*s\r\n",
		message.type, message.nodeID, message.length, message.length, message.content);
	}

PubKey_t returnSecondArg(NodeID_t nodeID, void* callbackArg) {
	return (PubKey_t) callbackArg;
	}

void* testInboundSocket(void* port, NodeInfo_t* selfInfo, NodeInfo_t* otherInfo) {
	SockFD_t sockfd = createSocket();

	SockAddr_t listenOn = getAddressWithIP("127.0.0.1",(unsigned int) port,0);

	int err = bindSocket(sockfd, listenOn);
	if(err != 1) {
		printf("Bind error: %i\r\n",err);
		exit(1);
		}
	else {
		printf("Bound to port %i\r\n",(unsigned int) port);
		}

	uint8_t* buf = malloc(sizeof(uint8_t)*BUFSIZE);
	while(1) {
		memset(buf,0,sizeof(uint8_t)*BUFSIZE);
		err = recvfrom(sockfd, buf, BUFSIZE, 0, (struct sockaddr*) NULL, 0);
		if (err < 0) {
			printf("recvfrom error: %i\r\n",err);
			}
		Message_t ret;
		decodeMessage(buf, err, selfInfo, &ret, &returnSecondArg, otherInfo->key ); 
		printf("Server received...\r\n");
		printMessage(ret); 
		}

	}

void testOutboundSocket(int port, NodeInfo_t* selfInfo, NodeInfo_t* otherInfo) {
	SockFD_t sockfd = createSocket();

	SockAddr_t sendTo = getAddressWithIP("127.0.0.1",port,0);
	
	printf("Building message...      ");
	Message_t message;
	char content[] = "Lorem ipsum dolor sit amet";
	message.type = MT_FIND;
	message.nodeID = (NodeID_t) 0x0102030405060708;
	message.content = (void*) content;
	message.length = strlen(content);
	printf("Done\r\n");

	printf("Encoding message...      ");
	uint8_t* encMessage;
	int encMessageLen;
	encodeMessage(message, selfInfo, otherInfo , &encMessage, &encMessageLen);
	printf("Done\r\n");
	//hexDump(encMessage,encMessageLen);

	sendto(sockfd, encMessage, encMessageLen, 0, (struct sockaddr*) &sendTo, sizeof(SockAddr_t));
	}

int main(int argc, char** argv) {
	Digest_t digest;
		
	RAND_seed(&(digest.data),DHTD_DIGEST_LENGTH);
		
	NodeInfo_t nodeInfo;
	NodeInfo_t otherInfo;

	int port = PORT;
	
	if( argc == 5) {
		printf("Generator mode\r\n");
		// Save public key to file
		printf("Generating RSA Keypair...");
		RSA* keyPair = RSA_generate_key(2048, 65537, NULL, NULL);
		printf("Done\r\n");

		printf("Building node info...    ");
		NodeInfo_t nodeInfo;
		nodeInfo.id = 0x0102030405060708;
		nodeInfo.key = (PubKey_t) keyPair;
		printf("Done\r\n");
		
		int err = saveConfig(port, nodeInfo, argv[4]);
		exit(1);
		}
	
	if( argc == 3 ) { // Is client...
		int err;
		printf("Client mode\r\n");
		err = openConfig(&port, &nodeInfo, argv[1]);
		err = openConfig(&port, &otherInfo, argv[2]);
		printf("Keys loaded...\r\n");
		while(1) {
			sleep(5);
			testOutboundSocket(port, &nodeInfo, &otherInfo);
			}
		}
	else { // Is server...
		int err;
		printf("Server mode\r\n");
		err = openConfig(&port, &nodeInfo, argv[1]);
		err = openConfig(&port, &otherInfo, argv[2]);
		printf("Keys loaded...\r\n");
		testInboundSocket((void*) port, &nodeInfo, &otherInfo);
		}

	exit(1);
	}
