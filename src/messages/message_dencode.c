#include "message.h"

#include <string.h>

#include <openssl/rand.h>

int encodeMessage(Message_t message, NodeInfo_t* sender, NodeInfo_t* recipient ,uint8_t** bytes, int* len) {
	uint8_t* plainBytes = NULL;
	int plainLen = 0;
	getBytesFromMessage(message, &plainBytes, &plainLen);
	
	signByteStream(plainBytes, plainLen, (SecKey_t) sender->key);

	// Get random key and IV
	SymKey_t key;
	RAND_bytes(key.data, sizeof( ((SymKey_t*)0)->data ) );
	SymIV_t iv;
	RAND_bytes(iv.data, sizeof( ((SymIV_t*)0)->data ) );
	
	uint8_t* encBytes = NULL;
	int encLen = 0;
	encryptByteStream((uint8_t*) plainBytes, plainLen, &key, &iv, &encBytes, &encLen);
	
	free(plainBytes);

	// Encrypt key and IV with recipient->key (pub)
	uint8_t* kivBytes;
	encryptKIV( &key, &iv, recipient->key, &kivBytes);

	// Append key and IV to encBytes to form message
	(*len) = encLen+DHTD_ENC_KIVPAIR_BYTE_LENGTH;
	(*bytes) = malloc(sizeof(uint8_t)*(*len));

	memcpy( &((*bytes)[0]), encBytes, encLen );
	memcpy( &((*bytes)[encLen]), kivBytes, DHTD_ENC_KIVPAIR_BYTE_LENGTH );

	free(encBytes);

	return 1;
	}

int decodeMessage(uint8_t* bytes, int len, NodeInfo_t* recipient, Message_t* message, PubKey_t (*getSenderPublicKey)(NodeID_t,void*), void* callbackArg ) {

	// Extract key and IV from bytes

	// Decrypt key and IV with recipient.key (priv)
	SymKey_t key;
	SymIV_t iv;

	decryptKIV( &(bytes[len-DHTD_ENC_KIVPAIR_BYTE_LENGTH]), (SecKey_t) recipient->key, &key, &iv);

	// Decrypt main bulk of message
	uint8_t* decBytes;
	int decLen;
	decryptByteStream(bytes, len-DHTD_ENC_KIVPAIR_BYTE_LENGTH, &key, &iv, &decBytes, &decLen);

	Message_t ret;
	getMessageFromBytes(decBytes, decLen, &ret); // Rather than fully unpacking here, use knowledge of packet to get sender?

	// Get senders public key
	PubKey_t senderKey = (*getSenderPublicKey)(ret.nodeID,callbackArg);

	int res = verifyByteStream(decBytes, decLen, senderKey );

	if ( res == 1 ) {
		(*message) = ret;
		}

	free(decBytes);	

	return 1;
	}
