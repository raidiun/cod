// config_nodeinfo.c

#include <string.h>

#include "config.h"

#define IP_ADDR_SIZE     4
#define PORT_NUMBER_SIZE 2

int bytesFromNodeInfo(NodeInfo_t nodeInfo, uint8_t** bytes, int* len, int privKey) {
	if( bytes == NULL ) { ERRORMSG("bytesFromNodeInfo: bytes is NULL\r\n"); return DHTD_ERROR; }
	if( len == NULL ) { ERRORMSG("bytesFromNodeInfo: len is NULL\r\n"); return DHTD_ERROR; }
	
	int err = 0;
	
	int baseLength = 0;
	
	if(privKey == 1) {
		baseLength = i2d_RSAPrivateKey(nodeInfo.key, NULL);
		}
	else {
		baseLength = i2d_RSAPublicKey(nodeInfo.key, NULL);
		}
	
	baseLength += IP_ADDR_SIZE + PORT_NUMBER_SIZE + sizeof(((NodeInfo_t*)0)->id) + sizeof(((NodeInfo_t*)0)->arrivalTime);
	
	(*bytes) = malloc(sizeof(uint8_t)*baseLength);
	
	int idx = 0;
	
	memcpy( &((*bytes)[idx]), &(nodeInfo.addr.sin_addr.s_addr) , IP_ADDR_SIZE );
	idx += IP_ADDR_SIZE;
	memcpy( &((*bytes)[idx]), &(nodeInfo.addr.sin_port) , PORT_NUMBER_SIZE );
	idx += PORT_NUMBER_SIZE;
	memcpy( &((*bytes)[idx]), &(nodeInfo.id) , sizeof(((NodeInfo_t*)0)->id) );
	idx += sizeof(((NodeInfo_t*)0)->id);
	memcpy( &((*bytes)[idx]), &(nodeInfo.arrivalTime) , sizeof(((NodeInfo_t*)0)->arrivalTime) );
	idx += sizeof(((NodeInfo_t*)0)->arrivalTime);

	uint8_t* bytesCopy = &((*bytes)[idx]);

	if(privKey == 1) {
		err = i2d_RSAPrivateKey(nodeInfo.key, (unsigned char**) &bytesCopy);
		}
	else {
		err = i2d_RSAPublicKey(nodeInfo.key, (unsigned char**) &bytesCopy);
		}
	
	if( err < 0 ) { ERRORMSG("Failed to encode key\r\n"); return DHTD_ERROR; }
	
	(*len) = idx + err;
	
	return 1;
	}

int nodeInfoFromBytes(uint8_t* bytes, int len, NodeInfo_t* nodeInfo, int privKey) {
	if( bytes == NULL )    { ERRORMSG("bytesFromNodeInfo: bytes is NULL\r\n");    return DHTD_ERROR; }
	if( len == 0 )         { ERRORMSG("bytesFromNodeInfo: len is zero\r\n");      return DHTD_ERROR; }
	if( nodeInfo == NULL ) { ERRORMSG("bytesFromNodeInfo: nodeInfo is NULL\r\n"); return DHTD_ERROR; }
	
	int err = 0;
	
	int idx = 0;
	
	memcpy( &(nodeInfo->addr.sin_addr.s_addr), &(bytes[idx]), IP_ADDR_SIZE );
	idx += IP_ADDR_SIZE;
	memcpy( &(nodeInfo->addr.sin_port), &(bytes[idx]), PORT_NUMBER_SIZE );
	idx += PORT_NUMBER_SIZE;
	memcpy(  &(nodeInfo->id), &(bytes[idx]), sizeof(((NodeInfo_t*)0)->id) );
	idx += sizeof(((NodeInfo_t*)0)->id);
	memcpy( &(nodeInfo->arrivalTime), &(bytes[idx]), sizeof(((NodeInfo_t*)0)->arrivalTime) );
	idx += sizeof(((NodeInfo_t*)0)->arrivalTime);
	
	uint8_t* bytesCopy = &(bytes[idx]);
	
	if(privKey == 1) {
		nodeInfo->key = d2i_RSAPrivateKey(NULL, (const unsigned char **) &bytesCopy, len-idx);
		}
	else {
		nodeInfo->key = d2i_RSAPublicKey(NULL, (const unsigned char **) &bytesCopy, len-idx);
		}
	
	if( nodeInfo->key == NULL ) { ERRORMSG("Failed to encode key\r\n"); return DHTD_ERROR; }
	
	return 1;
	}