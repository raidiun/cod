#include "routeHandler.h"

#define TABLE_DEPTH 16
#define TABLE_WIDTH 8

NodeInfo_t* routingTable[TABLE_WIDTH][TABLE_DEPTH];

uint8_t commonToBit(NodeID_t nodeA, NodeID_t nodeB) {
        uint8_t nc = 0;
        for( uint8_t pos = 0; pos < 64; pos++ ) {
		if( ((uint64_t) nodeA & 0x01 << (64-pos) )
                    != ((uint64_t) nodeB & 0x01 << (64-pos) ) ) {
			nc = 1;
			break;
			}
		}

	if( nc ) {
		return pos;
		}
	else {
		return 255;
		}
	}

int addNewRouteInfo(NodeInfo_t* nodeInfo) {
	uint8_t commonTo = commonToBit(nodeInfo->id, ownInfo.id);
	commonTo = commonTo / 8;

	NodeInfo_t* column[8] = routingTable[commonTo];

	for(nodeinfo in column) {
		check if NULL
		}

	if none NULL {
		replace oldest
		}

	}

int getRoute(NodeID_t id, NodeInfo_t* nodeInfo[TABLE_DEPTH]) {
	uint8_t commonTo = commonToBit(nodeInfo->id, ownInfo.id);
	commonTo = commonTo / 8;

	NodeInfo_t* column[8] = routingTable[commonTo];

	if(commonTo == 7) {
		// Check for exact matches...
		}
	else {
		int count = 0;
		for(int i = 0; i < 8; i++) {
			if(column[i] != NULL) {
				nodeInfo[count] = column[i];
				count++;
				}
			}
		return count;
		}
	}

