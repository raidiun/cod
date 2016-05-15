#include "crypto.h"

int encryptByteStream(uint8_t* bytes, int len, SymKey_t* key, SymIV_t* iv, uint8_t** encBytes, int* encLen) {
	if( bytes == NULL )    { ERRORMSG("encryptByteStream: bytes is NULL\r\n");    return DHTD_ERROR; }
	if( len == 0 )         { ERRORMSG("encryptByteStream: len is zero\r\n");      return DHTD_ERROR; }
	if( key == NULL )      { ERRORMSG("encryptByteStream: key is NULL\r\n");      return DHTD_ERROR; }
	if( iv == NULL )       { ERRORMSG("encryptByteStream: iv is NULL\r\n");       return DHTD_ERROR; }
	if( encBytes == NULL ) { ERRORMSG("encryptByteStream: encBytes is NULL\r\n"); return DHTD_ERROR; }
	if( encLen == NULL )   { ERRORMSG("encryptByteStream: encLen is NULL\r\n");   return DHTD_ERROR; }
	
	int err = 0;
	
	EVP_CIPHER_CTX cipherCtx;
	EVP_CIPHER_CTX_init(&cipherCtx);

	err = EVP_EncryptInit_ex(&cipherCtx, EVP_aes_256_cbc(), NULL, (unsigned char*) (key->data), (unsigned char*) (iv->data) );
	if( err == 0 ) {
		int rsaErr = ERR_peek_last_error();
		ERRORMSG("encryptByteStream: Error in EVP_EncryptInit_ex (%i: %s)\r\n",rsaErr,ERR_error_string(rsaErr,NULL));
		return DHTD_ERROR;
		}

	int encLength = len + EVP_MAX_BLOCK_LENGTH;
	(*encBytes) = malloc(sizeof(uint8_t)*encLength);
	if( (*encBytes) == NULL )   { ERRORMSG("encryptByteStream: NULL pointer allocated to encBytes\r\n");   return DHTD_ERROR; }

	int mainLen;

	err = EVP_EncryptUpdate(&cipherCtx, (unsigned char*) (*encBytes), &mainLen, (unsigned char*) bytes, len );
	if( err == 0 ) {
		int rsaErr = ERR_peek_last_error();
		ERRORMSG("encryptByteStream: Error in EVP_EncryptUpdate (%i: %s)\r\n",rsaErr,ERR_error_string(rsaErr,NULL));
		return DHTD_ERROR;
		}

	int endLen;

	EVP_EncryptFinal_ex(&cipherCtx, (unsigned char*) &((*encBytes)[mainLen]), &endLen );
	if( err == 0 ) {
		int rsaErr = ERR_peek_last_error();
		ERRORMSG("encryptByteStream: Error in EVP_EncryptFinal_ex (%i: %s)\r\n",rsaErr,ERR_error_string(rsaErr,NULL));
		return DHTD_ERROR;
		}
	
	(*encLen) = mainLen + endLen;

	err = EVP_CIPHER_CTX_cleanup(&cipherCtx);
	if( err == 0 ) {
		int rsaErr = ERR_peek_last_error();
		ERRORMSG("encryptByteStream: Error in EVP_CIPHER_CTX_cleanup (%i: %s)\r\n",rsaErr,ERR_error_string(rsaErr,NULL));
		return DHTD_ERROR;
		}

	return 1;	
	}

int decryptByteStream(uint8_t* bytes, int len, SymKey_t* key, SymIV_t* iv, uint8_t** decBytes, int* decLen) {
	if( bytes == NULL )    { ERRORMSG("decryptByteStream: bytes is NULL\r\n");    return DHTD_ERROR; }
	if( len == 0 )         { ERRORMSG("decryptByteStream: len is zero\r\n");      return DHTD_ERROR; }
	if( key == NULL )      { ERRORMSG("decryptByteStream: key is NULL\r\n");      return DHTD_ERROR; }
	if( iv == NULL )       { ERRORMSG("decryptByteStream: iv is NULL\r\n");       return DHTD_ERROR; }
	if( decBytes == NULL ) { ERRORMSG("decryptByteStream: decBytes is NULL\r\n"); return DHTD_ERROR; }
	if( decLen == NULL )   { ERRORMSG("decryptByteStream: decLen is NULL\r\n");   return DHTD_ERROR; }
	
	int err = 0;
	
	EVP_CIPHER_CTX cipherCtx;
	EVP_CIPHER_CTX_init(&cipherCtx);

	err = EVP_DecryptInit_ex(&cipherCtx, EVP_aes_256_cbc(), NULL, (unsigned char*) (key->data), (unsigned char*) (iv->data) );
	if( err == 0 ) {
		int rsaErr = ERR_peek_last_error();
		ERRORMSG("decryptByteStream: Error in EVP_DecryptInit_ex (%i: %s)\r\n",rsaErr,ERR_error_string(rsaErr,NULL));
		return DHTD_ERROR;
		}

	int decLength = len;
	(*decBytes) = malloc(sizeof(uint8_t)*decLength);
	if( (*decBytes) == NULL )   { ERRORMSG("decryptByteStream: NULL pointer allocated to decBytes\r\n");   return DHTD_ERROR; }

	int mainLen;

	err = EVP_DecryptUpdate(&cipherCtx, (unsigned char*) (*decBytes), &mainLen, (unsigned char*) bytes, len );
	if( err == 0 ) {
		int rsaErr = ERR_peek_last_error();
		ERRORMSG("decryptByteStream: Error in EVP_DecryptUpdate (%i: %s)\r\n",rsaErr,ERR_error_string(rsaErr,NULL));
		return DHTD_ERROR;
		}

	int endLen;
	
	err = EVP_DecryptFinal_ex(&cipherCtx, (unsigned char*) &((*decBytes)[mainLen]), &endLen );
	if( err == 0 ) {
		int rsaErr = ERR_peek_last_error();
		ERRORMSG("decryptByteStream: Error in EVP_DecryptFinal_ex (%i: %s)\r\n",rsaErr,ERR_error_string(rsaErr,NULL));
		return DHTD_ERROR;
		}

	(*decLen) = mainLen + endLen;

	err = EVP_CIPHER_CTX_cleanup(&cipherCtx);
	if( err == 0 ) {
		int rsaErr = ERR_peek_last_error();
		ERRORMSG("decryptByteStream: Error in EVP_CIPHER_CTX_cleanup (%i: %s)\r\n",rsaErr,ERR_error_string(rsaErr,NULL));
		return DHTD_ERROR;
		}

	return 1;
	}
