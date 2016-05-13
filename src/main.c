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

void hexDump(uint8_t* bytes, int len) {
	printf("XD:");
	for(int i=0; i<len; i++) {
		printf("%02x",bytes[i]);
		}
	printf("\r\n");
	}

void testAsymCrypto() {
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

	printf("Verifying signature...\r\n");
	int verify = verifySignature(digest, sig, (PubKey_t) keyPair);
	printf("Done, result: %i\r\n",verify);

	printf("Testing KIV Encryption...\r\n");

	SymKey_t key = {"abcdefghijklmnopqrstuvwxyz01234"};
	SymIV_t iv = {"0123456789abcde"};

	printf(" With K:\r\n");
	hexDump(key.data,DHTD_SYMKEY_BYTE_LENGTH);	
	printf(" With IV:\r\n");
	hexDump(iv.data,DHTD_SYMIV_BYTE_LENGTH);	

	uint8_t* kivBytes;

	printf("Encrypting KIV pair...\r\n");
	encryptKIV(&key, &iv, (PubKey_t) keyPair, &kivBytes);
	printf("Done\r\n");
	hexDump(kivBytes,DHTD_ENC_KIVPAIR_BYTE_LENGTH);	

	SymKey_t outKey;
	SymIV_t outIV;

	printf("Decrypting KIV pair...\r\n");
	decryptKIV(kivBytes, (SecKey_t) keyPair, &outKey, &outIV);
	printf("Done\r\n");

	printf(" Got K:\r\n");
	hexDump(outKey.data,DHTD_SYMKEY_BYTE_LENGTH);	
	printf(" Got IV:\r\n");
	hexDump(outIV.data,DHTD_SYMIV_BYTE_LENGTH);	

	}

void testSymCrypto() {
	char content[] = "Lorem ipsum dolor sit amet";

	SymKey_t key = {"abcdefghijklmnopqrstuvwxyz01234"};
	SymIV_t iv = {"0123456789abcde"};

	uint8_t* encBytes;
	int encLen;

	printf("Encrypting...\r\n");
	encryptByteStream((uint8_t*) content, strlen(content), &key, &iv, &encBytes, &encLen);
	printf("Done.\r\nGot bytes:\r\n");
	for( int i = 0; i < encLen; i++ ) {
		printf("%02x",encBytes[i]);
		}
	printf("\r\n");

	uint8_t* decBytes;
	int decLen;	

	printf("Decrypting...\r\n");
	decryptByteStream(encBytes, encLen, &key, &iv, &decBytes, &decLen);
	printf("Done.\r\nGot bytes:\r\n");
	for( int i = 0; i < decLen; i++ ) {
		printf("%c",decBytes[i]);
		}
	printf("\r\n");

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

void testFullMessagePath() {
	Digest_t digest;

	RAND_seed(&(digest.data),DHTD_DIGEST_LENGTH);

	printf("Generating RSA Keypair...");
	RSA* keyPair = RSA_generate_key(2048, 65537, NULL, NULL);
	printf("Done\r\n");

	printf("Building message...      ");
	Message_t message;
	char content[] = "Lorem ipsum dolor sit amet";
	message.type = MT_FIND;
	message.nodeID = (NodeID_t) 0x0102030405060708;
	message.content = (void*) content;
	message.length = strlen(content);
	printf("Done\r\n");
	
	printf("Enbyteing message...     ");
	uint8_t* bytes = NULL;
	int len = 0;
	getBytesFromMessage(message, &bytes, &len);
	printf("Done\r\n");
	hexDump(bytes,len);
	
	printf("Signing message...       ");
	signByteStream(bytes,len, (SecKey_t) keyPair);
	printf("Done\r\n");
	hexDump(bytes,len);

	printf("Encrypting message...    ");
	SymKey_t key = {"abcdefghijklmnopqrstuvwxyz01234"};
	SymIV_t iv = {"0123456789abcde"};
	uint8_t* encBytes;
	int encLen;
	encryptByteStream((uint8_t*) bytes, len, &key, &iv, &encBytes, &encLen);
	printf("Done\r\n");
	
	free(bytes);
	hexDump(encBytes,encLen);
	
	printf("Decrypting message...    ");
	uint8_t* decBytes;
	int decLen;
	decryptByteStream(encBytes, encLen, &key, &iv, &decBytes, &decLen);
	printf("Done\r\n");
	
	free(encBytes);
	
	printf("Verifying bytes...       ");
	int res = verifyByteStream(decBytes, decLen, (PubKey_t) keyPair);
	printf("Done (%i)\r\n",res);

	printf("Debyteing message...     ");
	Message_t ret;
	getMessageFromBytes(decBytes, decLen, &ret);
	printf("Done\r\n");
	
	free(decBytes);
	
	printf("\
Final message:\r\n\
 Type:    %i\r\n\
 nodeID:  %016llx\r\n\
 length:  %i\r\n\
 content: %.*s\r\n",
 ret.type, ret.nodeID, ret.length, ret.length, ret.content);

	}

PubKey_t returnSecondArg(NodeID_t nodeID, void* callbackArg) {
	return (PubKey_t) callbackArg;
	}

void testMessageDencode() {
	Digest_t digest;

	RAND_seed(&(digest.data),DHTD_DIGEST_LENGTH);

	printf("Generating RSA Keypair...");
	RSA* keyPair = RSA_generate_key(2048, 65537, NULL, NULL);
	printf("Done\r\n");

	printf("Building message...      ");
	Message_t message;
	char content[] = "Lorem ipsum dolor sit amet";
	message.type = MT_FIND;
	message.nodeID = (NodeID_t) 0x0102030405060708;
	message.content = (void*) content;
	message.length = strlen(content);
	printf("Done\r\n");

	printf("Message built:\r\n Type:    %i\r\n nodeID:  %016llx\r\n length:  %i\r\n content: %.*s\r\n",
		message.type, message.nodeID, message.length, message.length, message.content);

	printf("Building node info...    ");
	NodeInfo_t nodeInfo;
	nodeInfo.id = 0x0102030405060708;
	nodeInfo.key = (PubKey_t) keyPair;
	printf("Done\r\n");

	printf("Encoding message...      ");
	uint8_t* encMessage;
	int encMessageLen;
	encodeMessage(message, &nodeInfo, &nodeInfo , &encMessage, &encMessageLen);
	printf("Done\r\n");
	hexDump(encMessage,encMessageLen);
	
	printf("Decoding message...      ");
	Message_t ret;
	decodeMessage(encMessage, encMessageLen, &nodeInfo, &ret, &returnSecondArg, keyPair );
	printf("Done\r\n");
	
	free(encMessage);
	
	printf("\
Final message:\r\n\
 Type:    %i\r\n\
 nodeID:  %016llx\r\n\
 length:  %i\r\n\
 content: %.*s\r\n",
 ret.type, ret.nodeID, ret.length, ret.length, ret.content);

	}

int main(int argc, char** argv) {

	int port;
	if( argc == 2 ) {
		port = atoi(argv[1]);
		}
	else {
		port = PORT;
		}

//	testAsymCrypto();
//	testSymCrypto();
//	testMessaging();

//	testFullMessagePath();

	testMessageDencode();

//	testInboundSocket(port);

//	pthread_t listenerThread;
//	pthread_create(&listenerThread, NULL, &testInboundSocket, (void*) port);

//	sleep(1);
//	testOutboundSocket(port);

//	pause();
	
	exit(1);
	}
