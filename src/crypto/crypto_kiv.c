#include "crypto.h"

#include <string.h>

#define KIV_PAIR_LENGTH ( DHTD_SYMKEY_BYTE_LENGTH + DHTD_SYMIV_BYTE_LENGTH )

int encryptKIV(SymKey_t* key, SymIV_t* iv, PubKey_t pubKey, uint8_t** encBytes) {
	(*encBytes) = malloc(sizeof(uint8_t)*DHTD_ENC_KIVPAIR_BYTE_LENGTH);

	uint8_t kivPair[KIV_PAIR_LENGTH];
	memcpy( &(kivPair[0]), key->data, DHTD_SYMKEY_BYTE_LENGTH );
	memcpy( &(kivPair[DHTD_SYMIV_BYTE_LENGTH]), iv->data, DHTD_SYMIV_BYTE_LENGTH );

	RSA_public_encrypt(KIV_PAIR_LENGTH, (unsigned char*) kivPair, (unsigned char*) (*encBytes), (RSA*) pubKey, RSA_NO_PADDING);
	
	return 1;
	}

int decryptKIV(uint8_t* encBytes, SecKey_t secKey, SymKey_t* key, SymIV_t* iv) {
	uint8_t kivPair[KIV_PAIR_LENGTH];
	
	RSA_private_decrypt(DHTD_ENC_KIVPAIR_BYTE_LENGTH, (unsigned char*) encBytes, (unsigned char*) kivPair, (RSA*) secKey, RSA_NO_PADDING);

	memcpy( key->data, &(kivPair[0]), DHTD_SYMKEY_BYTE_LENGTH );
	memcpy( iv->data, &(kivPair[DHTD_SYMIV_BYTE_LENGTH]), DHTD_SYMIV_BYTE_LENGTH );

	return 1;
	}
