#include "../nodes.h"
#include "../debug.h"
//#include "../routes/routes.h"

typedef struct {
	NodeInfo_t nodeInfo;
	//RouteTable_t routes;
	} Config_t;

Config_t* getConfig();

int loadConfig(char* filename);

int saveConfig(char* filename);

int getNodeInfo(NodeInfo_t** nodeInfo);

//int getRoutes(RouteTable_t** routes);

int bytesFromNodeInfo(NodeInfo_t nodeInfo, uint8_t** bytes, int* len, int privKey);

int nodeInfoFromBytes(uint8_t* bytes, int len, NodeInfo_t* nodeInfo, int privKey);