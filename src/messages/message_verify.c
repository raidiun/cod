#include <stdlib.h>
#include <string.h>

#include "message.h"

int verifyByteStream(uint8_t* bytes, int len, PubKey_t pubKey) {
	if( bytes == NULL )  { ERRORMSG("verifyByteStream: bytes is NULL\r\n");  return DHTD_ERROR; }
	if( len == 0 )       { ERRORMSG("verifyByteStream: len is zero\r\n");    return DHTD_ERROR; }
	if( pubKey == NULL ) { ERRORMSG("verifyByteStream: pubKey is NULL\r\n"); return DHTD_ERROR; }
	
	Signature_t signature;
	memcpy( (void*) &(signature.data), (void*) &(bytes[len - SIGNATURE_LENGTH]), SIGNATURE_LENGTH );
	
	Digest_t digest = generateDigest( bytes, len-SIGNATURE_LENGTH );
	if( &(digest.data) == NULL ) { ERRORMSG("verifyByteStream: digest is NULL\r\n"); return DHTD_ERROR; }

	return verifySignature(digest,signature,pubKey);
	}

int signByteStream(uint8_t* bytes, int len, SecKey_t secKey) {
	if( bytes == NULL )  { ERRORMSG("signByteStream: bytes is NULL\r\n");  return DHTD_ERROR; }
	if( len == 0 )       { ERRORMSG("signByteStream: len is zero\r\n");    return DHTD_ERROR; }
	if( secKey == NULL ) { ERRORMSG("signByteStream: secKey is NULL\r\n"); return DHTD_ERROR; }
	
	Digest_t digest = generateDigest(bytes, len-SIGNATURE_LENGTH);
	if( &(digest.data) == NULL ) { ERRORMSG("signByteStream: digest is NULL\r\n"); return DHTD_ERROR; }
	
	Signature_t signature = generateSignature(digest,secKey);
	if( &(signature.data) == NULL ) { ERRORMSG("signByteStream: signature is NULL\r\n"); return DHTD_ERROR; }

	memcpy( (void*) &(bytes[len - SIGNATURE_LENGTH]), (void*) &signature, SIGNATURE_LENGTH );
	return 1;
	}
