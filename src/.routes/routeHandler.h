// Routing table maintainer

#ifndef DHTD_ROUTE_HANDLER_H
#define DHTD_ROUTE_HANDLER_H

#include <inttypes.h>

#include "../nodes.h"

#include "../sockets/sockets.h"

/*send(NodeID_t nodeID, Message_t message) {
	// Lookup nodeID
	// Encrypt message
	// Send packet
	}*/
// NodeInto_t holds the IP and port required to communicate with a node along with its ID

int addNewRouteInfo(NodeInfo_t* nodeInfo);

// getRoute provides the NodeInfo for the nearest node in the table
int getRoute(NodeID_t id, NodeInfo_t* nodeInfo);


#endif//DHTD_ROUTE_HANDLER_H

