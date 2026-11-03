#pragma once

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <iphlpapi.h>
#include <stdio.h>
#include "PacketHeader.h"

#pragma comment(lib, "Ws2_32.lib")

class NetWorkManager
{
public:
	NetWorkManager();	// コンストラクタ
	bool SuccessInit() const;	// 初期化に成功したかどうかを返す
	bool ConnectServer();	// サーバーと接続する
	template<typename T>
	bool RecvPacket(T& packet)
	{
		// ヘッダを受信してデータの種類を確認する
		PacketHeader header;
		if (RecvHeader(header) == false) return false;

		printf("ヘッダの受信成功\n");

		// 正しいパケットか調べる
		if (header.id != T::ID)
		{
			printf("異なるパケットIDを受信した\n");
			return false;
		}

		// パケットデータを受信する
		packet.header = header;
		char* data =
			reinterpret_cast<char*>(&packet);
		int totalRecv = 0;
		int size = header.packetSize - sizeof(PacketHeader);
		int iResult;

		// すべてのデータを受信するまで繰り返す
		while (totalRecv < size)
		{
			iResult = recv(
				connectSocket,
				data + sizeof(PacketHeader) + totalRecv,
				size - totalRecv,
				0);

			if (iResult == SOCKET_ERROR)
			{
				return false;
			}
			if (iResult == 0)
			{
				printf("接続切断\n");
				return false;
			}

			totalRecv += iResult;
			printf("受信したデータサイズ %d 中 %d\n", size, totalRecv);
		}

		return true;
	}

private:
	bool RecvHeader(PacketHeader& h);	// ヘッダを受信する

	WSADATA wsaData;	// ソケットの作成に必要な情報
	bool successInit;	// 初期化に成功したか
	SOCKET connectSocket;	// 接続用ソケット
};