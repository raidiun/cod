#include <openssl/pem.h>

#include "config.h"

int openConfig(int* port, NodeInfo_t* nodeInfo, char* filename) {
	FILE* configFile = fopen(filename, "r");
	DEBUGMSG("Opened file \"%s\" for reading \r\n", filename);
	nodeInfo->key = PEM_read_RSAPrivateKey(configFile, NULL, NULL, NULL );
	DEBUGMSG("RSA key read function called\r\n");
	fclose(configFile);
	
	if( nodeInfo->key == NULL ) {
		DEBUGMSG("RSA key read failed\r\n");
		return DHTD_ERROR;
		}
	
	return 1;
	}

int saveConfig(int port, NodeInfo_t nodeInfo, char* filename) {
	FILE* configFile = fopen(filename, "w");
	DEBUGMSG("Opened file \"%s\" for writing \r\n", filename);
	int err = PEM_write_RSAPrivateKey(configFile, (RSA*) nodeInfo.key, NULL, NULL, 0, NULL, NULL );
	DEBUGMSG("RSA key write function called\r\n");
	fclose(configFile);
	DEBUGMSG("File closed.\r\n");
	if( err == 0 ) {
		DEBUGMSG("RSA key write failed\r\n");
		return DHTD_ERROR;
		}
	DEBUGMSG("RSA key written\r\n");
	return 1;
	}

// write nodeInfo to file
/*	fscanf(configFile, "port: %i\n", port);	
	fscanf(configFile, "nodeID: 0x%016llx\n", &(nodeInfo->nodeID));
	fscanf(configFile, "", );

	int PEM_write_RSAPrivateKey(FILE *fp, RSA *x, const EVP_CIPHER *enc, unsigned char *kstr, int klen, pem_password_cb *cb, void *u);

*/

