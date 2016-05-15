#include "routes.h"

#include "../config/config.h"

#define NODE_ID_SIZE sizeof(NodeID_t)

NodeInfo_t* routingTable[DHTD_ROUTE_TABLE_WIDTH][DHTD_ROUTE_TABLE_DEPTH];

uint8_t commonToBit(NodeID_t nodeA, NodeID_t nodeB) {
	uint64_t diff = !(nodeA ^ nodeB);
	uint8_t idx = 0;
	while( (idx < NODE_ID_SIZE) && (diff & (0x01<<(NODE_ID_SIZE-idx))) ) {
		idx++;
		}
	return idx;
	}

int addNewRouteInfo(NodeInfo_t* nodeInfo) {
	Config_t* config = getConfig();
	
	uint8_t commonTo = commonToBit(nodeInfo->id, config->nodeInfo.id);
	commonTo = commonTo / (NODE_ID_SIZE/DHTD_ROUTE_TABLE_WIDTH);

	NodeInfo_t** column = routingTable[commonTo];

	int slotFound = 0;
	int oldestIdx = -1;
	uint64_t age = 0xFFFFFFFFFFFFFFFF;

	for(int i = 0; i < DHTD_ROUTE_TABLE_DEPTH; i++) {
		if( column[i] == NULL ) {
			column[i] = nodeInfo;
			slotFound = 1;
			break;
			}
		else {
			if( column[i]->arrivalTime < age ) {
				age = column[i]->arrivalTime;
				oldestIdx = i;
				}
			}
		}

	if( !slotFound ) {
		if( oldestIdx != -1 ) {
			column[oldestIdx] = nodeInfo;
			}
		else {
			return DHTD_ERROR;
			}
		}

	return 1;
	}

int getRoute(NodeID_t id, NodeInfo_t* nodeInfo[DHTD_ROUTE_TABLE_DEPTH]) {
	Config_t* config = getConfig();
	
	uint8_t commonTo = commonToBit(id, config->nodeInfo.id);
	commonTo = commonTo / (NODE_ID_SIZE/DHTD_ROUTE_TABLE_WIDTH);

	NodeInfo_t** column = routingTable[commonTo];

	if(commonTo == DHTD_ROUTE_TABLE_WIDTH) {
		// Check for exact matches...
		for(int i = 0; i < DHTD_ROUTE_TABLE_DEPTH; i++) {
			if((column[i]->id ^ id) == 0) {
				nodeInfo[0] = column[i];
				return 1;
				}
			}
		}
	
	int count = 0;
	for(int i = 0; i < DHTD_ROUTE_TABLE_DEPTH; i++) {
		if(column[i] != NULL) {
			nodeInfo[count] = column[i];
			count++;
			}
		}
	return count;
	}

