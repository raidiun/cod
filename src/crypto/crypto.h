#ifndef DHT_CRYPTO_H
#define DHT_CRYPTO_H

#include <inttypes.h>
#include "/usr/local/include/openssl/rsa.h"
#include "/usr/local/include/openssl/evp.h"

#include "../debug.h"

#define DHTD_RSA_MODULUS       2048
#define DHTD_RSA_MODULUS_BYTES (DHTD_RSA_MODULUS/8)

#define DHTD_SIGNATURE_LENGTH DHTD_RSA_MODULUS_BYTES

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


#define DHTD_SYMKEY_BYTE_LENGTH 32
#define DHTD_SYMIV_BYTE_LENGTH  16

typedef struct {
	uint8_t data[DHTD_SYMKEY_BYTE_LENGTH];
	} SymKey_t;

typedef struct {
	uint8_t data[DHTD_SYMIV_BYTE_LENGTH];
	} SymIV_t;

#define DHTD_ENC_KIVPAIR_BYTE_LENGTH DHTD_RSA_MODULUS_BYTES

Digest_t generateDigest(uint8_t* bytes, int len);

Signature_t generateSignature(Digest_t digest, SecKey_t secKey);

int verifySignature(Digest_t digest, Signature_t signature, PubKey_t pubKey);

int encryptByteStream(uint8_t* bytes, int len, SymKey_t* key, SymIV_t* iv, uint8_t** encBytes, int* encLen);
int decryptByteStream(uint8_t* bytes, int len, SymKey_t* key, SymIV_t* iv, uint8_t** decBytes, int* decLen);

int encryptKIV(SymKey_t* key, SymIV_t* iv, PubKey_t pubKey, uint8_t** encBytes);
int decryptKIV(uint8_t* encBytes, SecKey_t secKey, SymKey_t* key, SymIV_t* iv);

#endif//DHT_CRYPTO_H
