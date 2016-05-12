#ifndef DHT_CRYPTO_H
#define DHT_CRYPTO_H

#include <inttypes.h>
#include "/usr/local/include/openssl/rsa.h"

#include "../debug.h"

#define DHTD_SIGNATURE_LENGTH 256

typedef struct {
	uint8_t data[32];
	} Digest_t;

#define DHTD_DIGEST_LENGTH sizeof(((Digest_t*)0)->data)

typedef struct {
	uint8_t data[DHTD_SIGNATURE_LENGTH];
	} Signature_t;

typedef RSA*
	SecKey_t;

typedef RSA*
	PubKey_t;

Digest_t generateDigest(uint8_t* bytes, int len);

Signature_t generateSignature(Digest_t digest, SecKey_t secKey);

int verifySignature(Digest_t digest, Signature_t signature, PubKey_t pubKey);

#endif//DHT_CRYPTO_H
