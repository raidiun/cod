#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <unistd.h>
#include <pthread.h>

#include <arpa/inet.h>

#include "sockets/socket.h"
#include "messages/message.h"

#include "crypto/crypto.h"
#include "config/config.h"
#include <openssl/rand.h>
#include <openssl/rsa.h>

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
	printf("Message:\r\n");
	printf(" Type:    %i\r\n",message.type);
	printf(" ID:      %016llx\r\n",message.nodeID);
	printf(" Length:  %i\r\n",message.length);
	printf(" Content: %.*s\r\n",message.length, message.content);
	}

void printNodeInfo(NodeInfo_t nodeInfo) {
	printf("Node info:\r\n");
	printf(" Addr: %s\r\n",inet_ntoa(nodeInfo.addr.sin_addr));
	printf(" Port: %i\r\n",ntohs(nodeInfo.addr.sin_port));
	printf(" ID:   0x%016llx\r\n",nodeInfo.id);
	printf(" Arr:  0x%016llx\r\n",nodeInfo.arrivalTime);
	printf(" RSA:\r\n");
	RSA_print_fp(stdout, nodeInfo.key, 4);
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
	
	NodeInfo_t otherInfo;

	int port = PORT;
	
	if( argc == 5) {
		printf("Generator mode\r\n");
		// Save public key to file
		printf("Generating RSA Keypair...");
		RSA* keyPair = RSA_generate_key(2048, 65537, NULL, NULL);
		printf("Done\r\n");

		printf("Building address...      ");
		SockAddr_t address = getAddressWithIP("127.0.0.1",port,0);
		printf("Done\r\n");

		printf("Getting config...        ");
		Config_t* config = getConfig();
		printf("Done\r\n");

		printf("Building node info...    ");
		config->nodeInfo.addr = address;
		config->nodeInfo.id = 0x0102030405060708;
		config->nodeInfo.arrivalTime = 0x0000000000000001;
		config->nodeInfo.key = (PubKey_t) keyPair;
		printf("Done\r\n");
		
		printNodeInfo(config->nodeInfo);
		
		int err = saveConfig(argv[4]);
		exit(1);
		}
	if( argc == 2) {
		printf("Loading configuration... ");
		int err = loadConfig(argv[1]);
		printf("Done\r\n");
		
		printf("Getting config...        ");
		Config_t* config = getConfig();
		printf("Done\r\n");
		
		printNodeInfo(config->nodeInfo);
		}
	/*
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
*/
	exit(1);
	}
