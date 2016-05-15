#include <stdlib.h>
#include <string.h>

#include "message.h"

int getMessageFromBytes(uint8_t* bytes, int len, Message_t* message) {
	if( bytes == NULL )   { DEBUGMSG("getMessageFromBytes: bytes is NULL\r\n");   return DHTD_ERROR; }
	if( len == 0 )        { DEBUGMSG("getMessageFromBytes: len is zero\r\n");     return DHTD_ERROR; }	
	if( message == NULL ) { DEBUGMSG("getMessageFromBytes: message is NULL\r\n"); return DHTD_ERROR; }	
	
	// Copy message type
	memcpy( (void*) &(message->type),  (void*) &bytes[0], TYPE_LENGTH );

	// Copy sender ID
	memcpy( (void*) &(message->nodeID), (void*) &bytes[TYPE_LENGTH], SENDER_LENGTH );

	// Copy message length
	memcpy( (void*) &(message->length), (void*) &(bytes[TYPE_LENGTH+SENDER_LENGTH]), LENGTH_LENGTH );

	// Allocate message content pointer
	message->content = (uint8_t*) malloc(sizeof(uint8_t)*(message->length));
	if( message-> content == NULL ) { DEBUGMSG("getMessageFromBytes: NULL pointer allocated to message->content\r\n"); return DHTD_ERROR; }
	
	// Copy message content
	memcpy( (void*) message->content, (void*) &(bytes[TYPE_LENGTH+SENDER_LENGTH+LENGTH_LENGTH]), message->length );
	
	return 1;
	}

int getBytesFromMessage(Message_t message, uint8_t** bytes, int* len) {
	if( message.length != 0 && message.content == NULL ) {
		DEBUGMSG("getBytesFromMessage: message.content is NULL but message.length is non-zero\r\n");
		return DHTD_ERROR;
		}
	if( bytes == NULL )       { DEBUGMSG("getBytesFromMessage: bytes is NULL\r\n");   return DHTD_ERROR; }
	if( len == NULL )         { DEBUGMSG("getBytesFromMessage: len is NULL\r\n");     return DHTD_ERROR; }	
	
	// Calculate byte length
	*len = TYPE_LENGTH + SENDER_LENGTH + LENGTH_LENGTH + message.length + SIGNATURE_LENGTH;

	// Allocate array of bytes
	*bytes = (uint8_t*) malloc(sizeof(uint8_t)*(*len));
	if( *bytes == NULL ) { DEBUGMSG("getBytesFromMessage: NULL pointer allocated to bytes\r\n"); return DHTD_ERROR; }
	
	// Copy in type
	memcpy( (void*) &(*bytes)[0], (void*) &(message.type), TYPE_LENGTH );

	// Copy in sender ID
	memcpy( (void*) &(*bytes)[TYPE_LENGTH], (void*) &(message.nodeID), SENDER_LENGTH );

	// Copy in length
	memcpy( (void*) &(*bytes)[TYPE_LENGTH+SENDER_LENGTH], (void*) &(message.length), LENGTH_LENGTH );

	// Copy in content
	memcpy( (void*) &(*bytes)[TYPE_LENGTH+SENDER_LENGTH+LENGTH_LENGTH], (void*) message.content, (size_t) message.length );

	return 1;
	}
