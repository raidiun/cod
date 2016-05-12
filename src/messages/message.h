#ifndef DHTD_MESSAGE_H
#define DHTD_MESSAGE_H

#include <inttypes.h>

#include "../crypto/crypto.h"

#include "../nodes.h"

typedef uint8_t MessageType_t;
extern const MessageType_t MT_FIND;
extern const MessageType_t MT_SET;
extern const MessageType_t MT_KNOW;
extern const MessageType_t MT_ESTABLISH;
extern const MessageType_t MT_CHALLENGE;
extern const MessageType_t MT_RESPONSE;
extern const MessageType_t MT_ACK;
extern const MessageType_t MT_NACK;

typedef struct {
	MessageType_t type;
	NodeID_t nodeID;
	uint16_t length;
	void* content;
	} Message_t;

#define TYPE_LENGTH      sizeof(((Message_t*)0)->type)
#define SENDER_LENGTH    sizeof(((Message_t*)0)->nodeID)
#define LENGTH_LENGTH    sizeof(((Message_t*)0)->length)
#define SIGNATURE_LENGTH sizeof(Signature_t)

/* getMessageFromBytes: Parse a message from <bytes> of length <len> into Message_t structure <message>
    The byte sequence signature is not verified; see verifyByteStream
*/
int getMessageFromBytes(uint8_t* bytes, int len, Message_t* message);

/* getBytesFromMessage: Produce a byte sequence of length <*len> beginning at <*bytes> from <message>
    The byte sequence produced is not signed; see signByteStream
*/
int getBytesFromMessage(Message_t message, uint8_t** bytes, int* len);

/* verifyByteStream: Verify that the sequence <bytes> of length <len> has been signed by the private key corresponding to <pubKey>
*/
int verifyByteStream(uint8_t* bytes, int len, PubKey_t pubKey);

/* signByteStream: Sign the sequence <bytes> of length <len> with <privKey>
*/
int signByteStream(uint8_t* bytes, int len, SecKey_t privKey);

/* encodeMessage: Encode the <message> for a specific <recipient> from the <sender>.
    The result is of length <*len> and begins at <*bytes>
*/
int encodeMessage(Message_t message, NodeInfo_t* sender, NodeInfo_t* recipient ,uint8_t** bytes, int* len);

/* decodeMessage: Decode the byte stream of length <len> beginning at <bytes> sent to <recipient>.
    The result is placed in <message>
*/
int decodeMessage(uint8_t* bytes, int len, NodeInfo_t* recipient, Message_t* message);

#endif//DHTD_MESSAGE_H
