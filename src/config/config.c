#include <stdlib.h>

#include <openssl/pem.h>

#include "config.h"

Config_t* getConfig() {
	static Config_t* config = NULL;
	if( config == NULL ) {
		config = (Config_t*) malloc(sizeof(Config_t));
		if( config == NULL ) {
			ERRORMSG("getConfig: NULL pointer allocated to config\r\n");
			return NULL;
			}
		}
	return config;
	}

int loadConfig(char* filename) {
	if( filename == NULL ) { ERRORMSG("loadConfig: filename is NULL\r\n"); return DHTD_ERROR; }

	int err = 0;

	Config_t* config = getConfig();

	FILE* configFile = fopen(filename, "rb");
	if( configFile == NULL ) { ERRORMSG("loadConfig: Failed to open file \"%s\"\r\n",filename); return DHTD_ERROR; }
	DEBUGMSG("Loading config from file \"%s\"\r\n", filename);
	
	DEBUGMSG("Reading length from file...\r\n");
	int len;
	err = (int) fread(&len, sizeof(int), 1, configFile);
	if( ferror(configFile) ) { ERRORMSG("loadConfig: Error reading from configFile (%i)\r\n",ferror(configFile)); return DHTD_ERROR; }
	
	DEBUGMSG("Reading nodeInfo bytes from file...\r\n");
	uint8_t* bytes = malloc(sizeof(uint8_t)*len);
	err = (int) fread(bytes, sizeof(uint8_t), len, configFile);
	if( ferror(configFile) ) { ERRORMSG("loadConfig: Error reading from configFile (%i)\r\n",ferror(configFile)); return DHTD_ERROR; }
	
	DEBUGMSG("Debyteing nodeInfo...\r\n");
	err = nodeInfoFromBytes(bytes, len, &(config->nodeInfo), 1);
	if( err == DHTD_ERROR ) { ERRORMSG("loadConfig: nodeInfoFromBytes returned DHTD_ERROR\r\n"); return DHTD_ERROR; }
	
	fclose(configFile);
	DEBUGMSG("File closed.\r\n");
	
	return 1;
	}

int saveConfig(char* filename) {
	if( filename == NULL ) { ERRORMSG("saveConfig: filename is NULL\r\n"); return DHTD_ERROR; }

	int err = 0;

	Config_t* config = getConfig();

	FILE* configFile = fopen(filename, "wb");
	if( configFile == NULL ) { ERRORMSG("saveConfig: Failed to open file \"%s\"\r\n",filename); return DHTD_ERROR; }
	DEBUGMSG("Saving config to file \"%s\"\r\n", filename);
	
	uint8_t* bytes;
	int len;
	DEBUGMSG("Enbyteing nodeInfo...\r\n");
	err = bytesFromNodeInfo(config->nodeInfo, &bytes, &len, 1);
	if( err == DHTD_ERROR ) { ERRORMSG("saveConfig: bytesFromNodeInfo returned DHTD_ERROR\r\n"); return DHTD_ERROR; }
	
	DEBUGMSG("Writing length to file...\r\n");
	err = (int) fwrite(&len, sizeof(int), 1, configFile);
	if( ferror(configFile) ) { ERRORMSG("saveConfig: Error writing to configFile (%i)\r\n",ferror(configFile)); return DHTD_ERROR; }
	
	DEBUGMSG("Writing nodeInfo bytes to file...\r\n");
	err = (int) fwrite(bytes, sizeof(uint8_t), len, configFile);
	if( ferror(configFile) ) { ERRORMSG("saveConfig: Error writing to configFile (%i)\r\n",ferror(configFile)); return DHTD_ERROR; }
	
	free(bytes);
	
	// Write the routing table somehow...
	
	fclose(configFile);
	DEBUGMSG("File closed.\r\n");
	
	return 1;
	}
/*

What needs to be in an ESTABLISH message?
 - NodeID
 - IP
 - Port
 - Public Key

What does a node need to know on startup?
 - NodeID
 - IP
 - Port
 - Public Key
 - Private Key
 - Previous / Bootstrap Route Table

Rather than ascii as outlined below, use binary?
d2i_RSAPublicKey
Donwside is this is unencrypted, presumably there is a function
to handle that somewhere in the OpenSSL cryptolib...

NodeConfig.json
{
	"self":{
		"id": "0xFFFFFFFFFFFFFFFF",
		"ip": "255.255.255.255",
		"port": 65536,
		"key":"-----BEGIN RSA PRIVATE KEY-----\nA5ApLMRyvm9s6bW/MHK0QO0yZWtcNybO24FiOWlGRkqOKtRU3BpsYLWVgL/mZZNE\nMwKpAoGAWq6D61tzbYlwnKpUlzcejoxQ1Hq/x1U4WrDRf+jf5bzE7UJ2pvdHpXWn\n...\n48q+uyP4SWQO0JRVHNyNjWiGLMNuvDw9q7SJxvHG8+vOcUGaT+fhYQ7RTspvnqhl\n+/aGtUWYCZIjohagj0aCWSCXFsFLK3D87ZtYJqL4udBALnk2uis=\n-----END RSA PRIVATE KEY-----"
		}
	"nodes":[
		{
			"id": "0xFFFFFFFFFFFFFFFF",
			"ip": "255.255.255.255",
			"port": 65536,
			"key":"-----BEGIN RSA PUBLIC KEY-----\nA5ApLMRyvm9s6bW/MHK0QO0yZWtcNybO24FiOWlGRkqOKtRU3BpsYLWVgL/mZZNE\nMwKpAoGAWq6D61tzbYlwnKpUlzcejoxQ1Hq/x1U4WrDRf+jf5bzE7UJ2pvdHpXWn\n...\n48q+uyP4SWQO0JRVHNyNjWiGLMNuvDw9q7SJxvHG8+vOcUGaT+fhYQ7RTspvnqhl\n+/aGtUWYCZIjohagj0aCWSCXFsFLK3D87ZtYJqL4udBALnk2uis=\n-----END RSA PUBLIC KEY-----"
			},
		{
			"id": "0xFFFFFFFFFFFFFFFF",
			"ip": "255.255.255.255",
			"port": 65536,
			"key":"-----BEGIN RSA PUBLIC KEY-----\nA5ApLMRyvm9s6bW/MHK0QO0yZWtcNybO24FiOWlGRkqOKtRU3BpsYLWVgL/mZZNE\nMwKpAoGAWq6D61tzbYlwnKpUlzcejoxQ1Hq/x1U4WrDRf+jf5bzE7UJ2pvdHpXWn\n...\n48q+uyP4SWQO0JRVHNyNjWiGLMNuvDw9q7SJxvHG8+vOcUGaT+fhYQ7RTspvnqhl\n+/aGtUWYCZIjohagj0aCWSCXFsFLK3D87ZtYJqL4udBALnk2uis=\n-----END RSA PUBLIC KEY-----"
			}
		]
	}

All config based IO should fall to config/ * files
 Make a config struct?
 A global config to poll for info for ESTABLISH?
 
Challenge-Response:
 - Pick a secret K
 - Get H = hash(K)
 - Send CHALLENGE(H) to recipient
 - Recipient signs H and returns to sender: RESPONSE(Sign(H))
 - Sender verifies signature with pubkey from different source

hash there to prevent any periodicity in K being evident. Though K should be truly random
Problem being that all messages are signed so already have challenge / response framework??

New node Z, existing nodes known to Z: A,B
Z -> A: ESTABLISH(Z,IP,Port,PubKey)
Z -> B: ESTABLISH(Z,IP,Port,PubKey)
A -> Z: CHALLENGE(H)
Z -> A: RESPONSE(Sign(H))
// A verifies, Z marked with some form of trust by A
B -> Z: CHALLENGE(H)
Z -> B: RESPONSE(Sign(H))
// B verifies, Z marked with some form of trust by B


*/
// write nodeInfo to file
/*	fscanf(configFile, "port: %i\n", port);	
	fscanf(configFile, "nodeID: 0x%016llx\n", &(nodeInfo->nodeID));
	fscanf(configFile, "", );

	int PEM_write_RSAPrivateKey(FILE *fp, RSA *x, const EVP_CIPHER *enc, unsigned char *kstr, int klen, pem_password_cb *cb, void *u);

*/

