#include "crypto.h"

int encryptByteStream(uint8_t* bytes, int len, SymKey_t* key, SymIV_t* iv, uint8_t** encBytes, int* encLen) {
	EVP_CIPHER_CTX cipherCtx;
	EVP_CIPHER_CTX_init(&cipherCtx);

	EVP_EncryptInit_ex(&cipherCtx, EVP_aes_256_cbc(), NULL, (unsigned char*) (key->data), (unsigned char*) (iv->data) );

	int encLength = len + EVP_MAX_BLOCK_LENGTH;
	(*encBytes) = malloc(sizeof(uint8_t)*encLength);	

	int mainLen;

	EVP_EncryptUpdate(&cipherCtx, (unsigned char*) (*encBytes), &mainLen, (unsigned char*) bytes, len );

	int endLen;

	EVP_EncryptFinal_ex(&cipherCtx, (unsigned char*) &((*encBytes)[mainLen]), &endLen );
	
	(*encLen) = mainLen + endLen;

	EVP_CIPHER_CTX_cleanup(&cipherCtx);

	return 1;	
	}

int decryptByteStream(uint8_t* bytes, int len, SymKey_t* key, SymIV_t* iv, uint8_t** decBytes, int* decLen) {
	EVP_CIPHER_CTX cipherCtx;
	EVP_CIPHER_CTX_init(&cipherCtx);

	EVP_DecryptInit_ex(&cipherCtx, EVP_aes_256_cbc(), NULL, (unsigned char*) (key->data), (unsigned char*) (iv->data) );

	int decLength = len;
	(*decBytes) = malloc(sizeof(uint8_t)*decLength);

	int mainLen;

	EVP_DecryptUpdate(&cipherCtx, (unsigned char*) (*decBytes), &mainLen, (unsigned char*) bytes, len );

	int endLen;
	
	EVP_DecryptFinal_ex(&cipherCtx, (unsigned char*) &((*decBytes)[mainLen]), &endLen );

	(*decLen) = mainLen + endLen;

	EVP_CIPHER_CTX_cleanup(&cipherCtx);

	return 1;
	}
