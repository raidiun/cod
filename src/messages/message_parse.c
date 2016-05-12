#include <stdlib.h>
#include <string.h>

#include "message.h"

int getMessageFromBytes(uint8_t* bytes, int len, Message_t* message) {
	// Get message type
	memcpy( (void*) &(message->type),  (void*) &bytes[0], TYPE_LENGTH );

	// Get sender ID
	memcpy( (void*) &(message->nodeID), (void*) &bytes[TYPE_LENGTH], SENDER_LENGTH );

	// Get message length
	memcpy( (void*) &(message->length), (void*) &(bytes[TYPE_LENGTH+SENDER_LENGTH]), LENGTH_LENGTH );

	message->content = (uint8_t*) malloc(sizeof(uint8_t)*(message->length));
	memcpy( (void*) message->content, (void*) &(bytes[TYPE_LENGTH+SENDER_LENGTH+LENGTH_LENGTH]), message->length );
	
	return 1;
	}

int getBytesFromMessage(Message_t message, uint8_t** bytes, int* len) {
	// Calculate byte length
	*len = TYPE_LENGTH + SENDER_LENGTH + LENGTH_LENGTH + message.length + SIGNATURE_LENGTH;

	// Allocate array of bytes
	*bytes = (uint8_t*) malloc(sizeof(uint8_t)*(*len));

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
