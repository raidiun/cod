#ifndef DHT_NODES_H
#define DHT_NODES_H

#include <inttypes.h>

#include "sockets/socket.h"
#include "crypto/crypto.h"

typedef uint64_t
	NodeID_t;

typedef struct {
	SockAddr_t addr;
	NodeID_t id;
	int64_t arrivalTime;
	PubKey_t key;
	} NodeInfo_t;

#endif//DHT_NODES_H
