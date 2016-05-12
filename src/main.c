#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <unistd.h>
#include <pthread.h>

#include "sockets/socket.h"
#include "messages/message.h"

#include "crypto/crypto.h"
#include <openssl/rand.h>

#define PORT 9898
#define BUFSIZE 1024

void testCrypto() {
	Digest_t digest;

	RAND_seed(&(digest.data),DHTD_DIGEST_LENGTH);

	printf("Generating RSA Keypair...\r\n");
	RSA* keyPair = RSA_generate_key(2048, 65537, NULL, NULL);
	printf("Done\r\n");

	printf("Digest:\r\n");
	for(int i = 0; i<DHTD_DIGEST_LENGTH; i++) {
		printf("%02x",digest.data[i]);
		}
	printf("\r\n");

	printf("Generating Signature...\r\n");
	Signature_t sig = generateSignature(digest, (SecKey_t) keyPair);
	printf("Done, Signature:\r\n");
	for(int i = 0; i<DHTD_SIGNATURE_LENGTH; i++) {
		printf("%02x",sig.data[i]);
		}
	printf("\r\n");

	printf("Verifying signature...");
	int verify = verifySignature(digest, sig, (PubKey_t) keyPair);
	printf("Done, result: %i\r\n",verify);
	}

void testMessaging() {
	printf("Building message...");
	Message_t message;

	message.type = MT_FIND;
	message.nodeID = (NodeID_t) 0x0102030405060708;

	char content[] = "Lorem ipsum dolor sit amet";

	message.content = (void*) content;
	message.length = strlen(content);

	printf("Done. Content: %s\r\nConverting to bytes...",(char*) message.content);

	uint8_t* bytes = NULL;
	int len = 0;
	getBytesFromMessage(message, &bytes, &len);

	printf("Done\r\nGot bytes:\r\n");
	for(int i = 0; i < len; i++ ) {
		printf("%02x",bytes[i]);
		}
	printf("\r\n");

	printf("Generating RSA Keypair...\r\n");
	RSA* keyPair = RSA_generate_key(2048, 65537, NULL, NULL);
	printf("Done.\r\n");

	printf("Adding signature...\r\n");
	signByteStream(bytes,len, (SecKey_t) keyPair);
	printf("Done\r\nGot bytes:\r\n");
	for(int i = 0; i < len; i++ ) {
		printf("%02x",bytes[i]);
		}
	printf("\r\n");

	printf("Verifying bytes...");
	int res = verifyByteStream(bytes,len, (PubKey_t) keyPair);
	printf("Done. Result: %i\r\n",res);

	printf("Converting back to Message_t...\r\n");
	Message_t ret;
	getMessageFromBytes(bytes, len, &ret);
	printf("Done.\r\nGot:\r\n Content: %s\r\n NodeID: %016llx\r\n",ret.content,ret.nodeID);
		
	}

void* testInboundSocket(void* port) {
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

void testOutboundSocket(int port) {
	SockFD_t sockfd = createSocket();

	SockAddr_t sendTo = getAddressWithIP("127.0.0.1",port,0);

	char message[] = "Lorem ipsum dolor sit amet";

	sendto(sockfd, message, strlen(message), 0, (struct sockaddr*) &sendTo, sizeof(SockAddr_t));
	}

int main(int argc, char** argv) {

	int port;
	if( argc == 2 ) {
		port = atoi(argv[1]);
		}
	else {
		port = PORT;
		}

	testCrypto();
	testMessaging();
//	testInboundSocket(port);

	pthread_t listenerThread;
	pthread_create(&listenerThread, NULL, &testInboundSocket, (void*) port);

	sleep(1);
	testOutboundSocket(port);
	
	pause();
	
	exit(1);
	}
