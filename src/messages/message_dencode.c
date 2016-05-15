#include "message.h"

#include <string.h>

#include <openssl/rand.h>
#include <openssl/err.h>

int encodeMessage(Message_t message, NodeInfo_t* sender, NodeInfo_t* recipient, uint8_t** bytes, int* len) {
	if( message.length != 0 && message.content == NULL ) {
		DEBUGMSG("encodeMessage: message.content is NULL but message.length is non-zero\r\n");
		return -1;
		}	
	if( sender == NULL )    { DEBUGMSG("encodeMessage: sender is NULL\r\n");    return DHTD_ERROR; }
	if( recipient == NULL ) { DEBUGMSG("encodeMessage: recipient is NULL\r\n"); return DHTD_ERROR; }
	if( bytes == NULL )     { DEBUGMSG("encodeMessage: bytes is NULL\r\n");     return DHTD_ERROR; }
	if( len == NULL )       { DEBUGMSG("encodeMessage: len is NULL\r\n");       return DHTD_ERROR; }
	
	int err = 0;
	
	uint8_t* plainBytes = NULL;
	int plainLen = 0;
	err = getBytesFromMessage(message, &plainBytes, &plainLen);
	if( err == DHTD_ERROR ) { DEBUGMSG("encodeMessage: getBytesFromMessage returned DHTD_ERROR\r\n"); return DHTD_ERROR; }
	
	err = signByteStream(plainBytes, plainLen, (SecKey_t) sender->key);
	if( err == DHTD_ERROR ) { DEBUGMSG("encodeMessage: signByteStream returned DHTD_ERROR\r\n"); return DHTD_ERROR; }

	// Get random key and IV
	SymKey_t key;
	err = RAND_bytes(key.data, sizeof( ((SymKey_t*)0)->data ) );
	if( err == 0 ) {
		int rsaErr = ERR_peek_last_error();
		DEBUGMSG("encodeMessage: Error in RAND_bytes when getting key (%i: %s)\r\n",rsaErr,ERR_error_string(rsaErr,NULL));
		return DHTD_ERROR;
		}
	SymIV_t iv;
	RAND_bytes(iv.data, sizeof( ((SymIV_t*)0)->data ) );
	if( err == 0 ) {
		int rsaErr = ERR_peek_last_error();
		DEBUGMSG("encodeMessage: Error in RAND_bytes when getting iv (%i: %s)\r\n",rsaErr,ERR_error_string(rsaErr,NULL));
		return DHTD_ERROR;
		}
	
	uint8_t* encBytes = NULL;
	int encLen = 0;
	err = encryptByteStream((uint8_t*) plainBytes, plainLen, &key, &iv, &encBytes, &encLen);
	if( err == DHTD_ERROR ) { DEBUGMSG("encodeMessage: encryptByteStream returned DHTD_ERROR\r\n"); return DHTD_ERROR; }
	
	free(plainBytes);

	// Encrypt key and IV with recipient->key (pub)
	uint8_t* kivBytes;
	err = encryptKIV( &key, &iv, recipient->key, &kivBytes);
	if( err == DHTD_ERROR ) { DEBUGMSG("encodeMessage: encryptKIV returned DHTD_ERROR\r\n"); return DHTD_ERROR; }

	// Append key and IV to encBytes to form message
	(*len) = encLen+DHTD_ENC_KIVPAIR_BYTE_LENGTH;
	(*bytes) = malloc(sizeof(uint8_t)*(*len));
	if( (*bytes) == NULL ) { DEBUGMSG("encodeMessage: NULL pointer allocated to bytes\r\n"); return DHTD_ERROR; }

	memcpy( &((*bytes)[0]), encBytes, encLen );
	memcpy( &((*bytes)[encLen]), kivBytes, DHTD_ENC_KIVPAIR_BYTE_LENGTH );

	free(encBytes);

	return 1;
	}

int decodeMessage(uint8_t* bytes, int len, NodeInfo_t* recipient, Message_t* message, PubKey_t (*getSenderPublicKey)(NodeID_t,void*), void* callbackArg ) {
	if( bytes == NULL )              { DEBUGMSG("decodeMessage: bytes is NULL\r\n");                       return DHTD_ERROR; }
	if( len == 0 )                   { DEBUGMSG("decodeMessage: len is zero\r\n");                         return DHTD_ERROR; }
	if( recipient == NULL )          { DEBUGMSG("decodeMessage: recipient is NULL\r\n");                   return DHTD_ERROR; }
	if( message == NULL )            { DEBUGMSG("decodeMessage: message is NULL\r\n");                     return DHTD_ERROR; }
	if( getSenderPublicKey == NULL ) { DEBUGMSG("decodeMessage: getSenderPublicKey callback is NULL\r\n"); return DHTD_ERROR; }
	
	int err = 0;
	
	SymKey_t key;
	SymIV_t iv;

	err = decryptKIV( &(bytes[len-DHTD_ENC_KIVPAIR_BYTE_LENGTH]), (SecKey_t) recipient->key, &key, &iv);
	if( err == DHTD_ERROR ) { DEBUGMSG("decodeMessage: decryptKIV returned DHTD_ERROR\r\n"); return DHTD_ERROR; }
	
	// Decrypt main bulk of message
	uint8_t* decBytes;
	int decLen;
	err = decryptByteStream(bytes, len-DHTD_ENC_KIVPAIR_BYTE_LENGTH, &key, &iv, &decBytes, &decLen);
	if( err == DHTD_ERROR ) { DEBUGMSG("decodeMessage: decryptByteStream returned DHTD_ERROR\r\n"); return DHTD_ERROR; }

	Message_t ret;
	err = getMessageFromBytes(decBytes, decLen, &ret); // Rather than fully unpacking here, use knowledge of packet to get sender?
	if( err == DHTD_ERROR ) { DEBUGMSG("decodeMessage: getMessageFromBytes returned DHTD_ERROR\r\n"); return DHTD_ERROR; }

	// Get senders public key
	PubKey_t senderKey = (*getSenderPublicKey)(ret.nodeID,callbackArg);
	if( senderKey == NULL ) { DEBUGMSG("decodeMessage: getSenderPublicKey callback returned NULL\r\n"); return DHTD_ERROR; }

	err = verifyByteStream(decBytes, decLen, senderKey );
	if( err == DHTD_ERROR ) { DEBUGMSG("decodeMessage: verifyByteStream returned DHTD_ERROR\r\n"); return DHTD_ERROR; }

	if ( err == 1 ) {
		(*message) = ret;
		}
	else {
		DEBUGMSG("decodeMessage: byte stream could not be verified\r\n");
		return DHTD_ERROR;
		}

	free(decBytes);	

	return 1;
	}
