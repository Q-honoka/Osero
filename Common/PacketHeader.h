#pragma once

#include <cstdint>
#include "PacketID.h"

/*
*	すべてのパケットに共通して存在するデータ
*/

struct PacketHeader
{
	PacketID id;	// パケットの種類
	uint16_t packetSize;	// パケットの大きさ
};