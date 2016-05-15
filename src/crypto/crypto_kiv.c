#include "crypto.h"

#include <string.h>

#define KIV_PAIR_LENGTH ( DHTD_SYMKEY_BYTE_LENGTH + DHTD_SYMIV_BYTE_LENGTH )

#define KIV_PADDING RSA_PKCS1_OAEP_PADDING

int encryptKIV(SymKey_t* key, SymIV_t* iv, PubKey_t pubKey, uint8_t** encBytes) {
	if( key == NULL )      { DEBUGMSG("encryptKIV: key is NULL\r\n");      return DHTD_ERROR; }
	if( iv == NULL )       { DEBUGMSG("encryptKIV: iv is NULL\r\n");       return DHTD_ERROR; }
	if( pubKey == NULL )   { DEBUGMSG("encryptKIV: pubKey is NULL\r\n");   return DHTD_ERROR; }
	if( encBytes == NULL ) { DEBUGMSG("encryptKIV: encBytes is NULL\r\n"); return DHTD_ERROR; }
	
	int err = 0;
	
	(*encBytes) = malloc(sizeof(uint8_t)*DHTD_ENC_KIVPAIR_BYTE_LENGTH);
	if( (*encBytes) == NULL ) { DEBUGMSG("encryptKIV: NULL pointer allocated to encBytes\r\n"); return DHTD_ERROR; }

	uint8_t kivPair[KIV_PAIR_LENGTH];
	memcpy( &(kivPair[0]), key->data, DHTD_SYMKEY_BYTE_LENGTH );
	memcpy( &(kivPair[DHTD_SYMKEY_BYTE_LENGTH]), iv->data, DHTD_SYMIV_BYTE_LENGTH );

	err = RSA_public_encrypt(KIV_PAIR_LENGTH, (unsigned char*) kivPair, (unsigned char*) (*encBytes), (RSA*) pubKey, KIV_PADDING);
	if( err == -1 ) {
		int rsaErr = ERR_peek_last_error();
		DEBUGMSG("encryptKIV: Error in RSA_public_encrypt (%i: %s)\r\n",rsaErr,ERR_error_string(rsaErr,NULL));
		return DHTD_ERROR;
		}
	
	return 1;
	}

int decryptKIV(uint8_t* encBytes, SecKey_t secKey, SymKey_t* key, SymIV_t* iv) {
	if( encBytes == NULL ) { DEBUGMSG("decryptKIV: encBytes is NULL\r\n"); return DHTD_ERROR; }
	if( secKey == NULL )   { DEBUGMSG("decryptKIV: secKey is NULL\r\n");   return DHTD_ERROR; }
	if( key == NULL )      { DEBUGMSG("decryptKIV: key is NULL\r\n");      return DHTD_ERROR; }
	if( iv == NULL )       { DEBUGMSG("decryptKIV: iv is NULL\r\n");       return DHTD_ERROR; }
	
	int err = 0;
	
	uint8_t kivPair[KIV_PAIR_LENGTH];
	
	err = RSA_private_decrypt(DHTD_ENC_KIVPAIR_BYTE_LENGTH, (unsigned char*) encBytes, (unsigned char*) kivPair, (RSA*) secKey, KIV_PADDING);
	if( err == -1 ) {
		int rsaErr = ERR_peek_last_error();
		DEBUGMSG("decryptKIV: Error in RSA_private_decrypt (%i: %s)\r\n",rsaErr,ERR_error_string(rsaErr,NULL));
		return DHTD_ERROR;
		}

	memcpy( key->data, &(kivPair[0]), DHTD_SYMKEY_BYTE_LENGTH );
	memcpy( iv->data, &(kivPair[DHTD_SYMKEY_BYTE_LENGTH]), DHTD_SYMIV_BYTE_LENGTH );

	return 1;
	}
