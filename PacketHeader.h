#include "PacketID.h"

/*
*	すべてのパケットに共通して存在するデータ
*/

struct PacketHeader
{
	PacketID id;	// パケットの種類
};