#include <stdlib.h>
#include <string.h>

#include "message.h"

int verifyByteStream(uint8_t* bytes, int len, PubKey_t pubKey) {
	Signature_t signature;
	memcpy( (void*) &signature, (void*) &(bytes[len - SIGNATURE_LENGTH]), SIGNATURE_LENGTH );
	
	Digest_t digest = generateDigest( bytes, len-SIGNATURE_LENGTH );

	return verifySignature(digest,signature,pubKey);
	}

int signByteStream(uint8_t* bytes, int len, SecKey_t privKey) {
	Digest_t digest = generateDigest(bytes, len-SIGNATURE_LENGTH);
	Signature_t signature = generateSignature(digest,privKey);

	memcpy( (void*) &(bytes[len - SIGNATURE_LENGTH]), (void*) &signature, SIGNATURE_LENGTH );
	return 1;
	}
