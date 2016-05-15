#include "crypto.h"

#define HASH_ALGO NID_sha256

Digest_t NullDigest() {
	Digest_t* result = NULL;
	return (*result);
	}

Signature_t NullSignature() {
	Signature_t* result = NULL;
	return (*result);
	}

// OpenSSL 1.1 stuff removed, can use static context...
Digest_t generateDigest(uint8_t* bytes, int len) {
	
	if( bytes == NULL) { DEBUGMSG("generateDigest: bytes is NULL\r\n"); return NullDigest(); }
	if( len == 0)      { DEBUGMSG("generateDigest: len is zero\r\n");   return NullDigest(); }
	
	int err = 0;
	
	EVP_MD_CTX* mdctx = EVP_MD_CTX_create();
	
	if ( mdctx == NULL ) {
		DEBUGMSG("generateDigest: Error getting digest context\r\n");
		return NullDigest();
		}
		
	static const EVP_MD* md = NULL;
	
	if ( md == NULL ) {
//		md = EVP_get_digestbynid(HASH_ALGO);
//		md = EVP_get_digestbyname("sha256");
		md = EVP_sha256();
		if( md == NULL ) {
			DEBUGMSG("generateDigest: Error getting digest structure\r\n");
			return NullDigest();
			}
		}
	
	Digest_t digest;
	unsigned int md_len;

	err = EVP_DigestInit_ex(mdctx, md, NULL);
	if( err == 0 ) {
		int rsaErr = ERR_peek_last_error();
		DEBUGMSG("generateDigest: Error in EVP_DigestInit_ex (%i: %s)\r\n",rsaErr,ERR_error_string(rsaErr,NULL));
		return NullDigest();
		}
	
	err = EVP_DigestUpdate(mdctx, bytes, len);
	if( err == 0 ) {
		int rsaErr = ERR_peek_last_error();
		DEBUGMSG("generateDigest: Error in EVP_DigestUpdate (%i: %s)\r\n",rsaErr,ERR_error_string(rsaErr,NULL));
		return NullDigest();
		}
		
	err = EVP_DigestFinal_ex(mdctx, (unsigned char*) &(digest.data), &md_len);
	if( err == 0 ) {
		int rsaErr = ERR_peek_last_error();
		DEBUGMSG("generateDigest: Error in EVP_DigestFinal_ex (%i: %s)\r\n",rsaErr,ERR_error_string(rsaErr,NULL));
		return NullDigest();
		}

	EVP_MD_CTX_destroy(mdctx);

	return digest;
	}

Signature_t generateSignature(Digest_t digest, SecKey_t secKey) {
	if( &(digest.data) == NULL ) { DEBUGMSG("generateSignature: &(digest.data) is NULL\r\n"); return NullSignature(); }
	if( secKey == NULL )         { DEBUGMSG("generateSignature: secKey is NULL\r\n");         return NullSignature(); }
	
	int err = 0;
	
	Signature_t signature;
	unsigned int siglen;
	err = RSA_sign(HASH_ALGO, (unsigned char*) &(digest.data), DHTD_DIGEST_LENGTH, (unsigned char*) &(signature.data), &siglen, (RSA*) secKey);
	if( err == 0 ) {
		int rsaErr = ERR_peek_last_error();
		DEBUGMSG("generateSignature: Error in RSA_sign (%i: %s)\r\n",rsaErr,ERR_error_string(rsaErr,NULL));
		return NullSignature();
		}

	return signature;
	}

int verifySignature(Digest_t digest, Signature_t signature, PubKey_t pubKey) {
	if( &(digest.data) == NULL )    { DEBUGMSG("verifySignature: &(digest.data) is NULL\r\n");    return DHTD_ERROR; }
	if( &(signature.data) == NULL ) { DEBUGMSG("verifySignature: &(signature.data) is NULL\r\n"); return DHTD_ERROR; }
	if( pubKey == NULL )            { DEBUGMSG("verifySignature: pubKey is NULL\r\n");            return DHTD_ERROR; }
	
	int err = 0;
	
	err = RSA_verify(HASH_ALGO, (const unsigned char*) digest.data, DHTD_DIGEST_LENGTH, (unsigned char*) signature.data, DHTD_SIGNATURE_LENGTH, (RSA*) pubKey);
	
	if( err == 0 ) {
		int rsaErr = ERR_peek_last_error();
		DEBUGMSG("verifySignature: Error in RSA_verify (%i: %s)\r\n",rsaErr,ERR_error_string(rsaErr,NULL));
		return 0;
		}
	
	return 1;
	}
