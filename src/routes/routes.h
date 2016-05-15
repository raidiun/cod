// Routing table maintainer

#ifndef DHTD_ROUTE_HANDLER_H
#define DHTD_ROUTE_HANDLER_H

#define DHTD_ROUTE_TABLE_DEPTH 16
#define DHTD_ROUTE_TABLE_WIDTH 8

#include <inttypes.h>

#include "../debug.h"

#include "../nodes.h"

#include "../sockets/socket.h"

/*send(NodeID_t nodeID, Message_t message) {
	// Lookup nodeID
	// Encrypt message
	// Send packet
	}*/
// NodeInto_t holds the IP and port required to communicate with a node along with its ID

int addNewRouteInfo(NodeInfo_t* nodeInfo);

// getRoute provides the NodeInfo for the nearest node in the table
int getRoute(NodeID_t id, NodeInfo_t* nodeInfo[DHTD_ROUTE_TABLE_DEPTH]);


#endif//DHTD_ROUTE_HANDLER_H

