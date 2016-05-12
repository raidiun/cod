#include "crypto.h"

#include "/usr/local/include/openssl/evp.h"
#include "/usr/local/include/openssl/rsa.h"

#define HASH_ALGO NID_sha256

#ifdef OPENSSL_1_1
Digest_t generateDigest(uint8_t* bytes, int len) {
	static const EVP_MD_CTX* mdctx = NULL;
	static const EVP_MD* md = NULL;
	
	if ( mdctx == NULL ) {
		mdctx = EVP_MD_CTX_new();
		if( mdctx == NULL ) { ERROREXIT("generateDigest: Error getting digest context.",5); }
		}

	if ( md == NULL ) {
		md = EVP_get_digestbynid(HASH_ALGO);
		if( md == NULL ) { ERROREXIT("generateDigest: Error getting digest structure.",5); }
		}
	
	Digest_t md_value;
	int md_len, i;


	EVP_MD_CTX_reset(mdctx);
	EVP_DigestInit_ex(mdctx, md, NULL);
	EVP_DigestUpdate(mdctx, bytes, len);
	EVP_DigestFinal_ex(mdctx, (unsigned char*) &(md_value.data), &md_len);

	return md_value;
	}
#else
Digest_t generateDigest(uint8_t* bytes, int len) {
	EVP_MD_CTX* mdctx = EVP_MD_CTX_create();
	static const EVP_MD* md = NULL;
	
	if ( mdctx == NULL ) {
		if( mdctx == NULL ) { ERROREXIT("generateDigest: Error getting digest context.",5); }
		}

	if ( md == NULL ) {
//		md = EVP_get_digestbynid(HASH_ALGO);
//		md = EVP_get_digestbyname("sha256");
		md = EVP_sha256();
		if( md == NULL ) { ERROREXIT("generateDigest: Error getting digest structure.",5); }
		}
	
	Digest_t digest;
	unsigned int md_len;

	EVP_DigestInit_ex(mdctx, md, NULL);
	EVP_DigestUpdate(mdctx, bytes, len);
	EVP_DigestFinal_ex(mdctx, (unsigned char*) &(digest.data), &md_len);

	EVP_MD_CTX_destroy(mdctx);

	return digest;
	}
#endif

Signature_t generateSignature(Digest_t digest, SecKey_t secKey) {
	//uint8_t* signature = (uint8_t*) malloc(sizeof(uint8_t)*RSA_size((RSA*) secKey));
	Signature_t signature;
	unsigned int siglen;
	RSA_sign(HASH_ALGO, (unsigned char*) &(digest.data), 32, (unsigned char*) &(signature.data), &siglen, (RSA*) secKey);

	return signature;
	}

int verifySignature(Digest_t digest, Signature_t signature, PubKey_t pubKey) {
	return RSA_verify(HASH_ALGO, (const unsigned char*) digest.data, DHTD_DIGEST_LENGTH, (unsigned char*) signature.data, DHTD_SIGNATURE_LENGTH, (RSA*) pubKey);
	}
