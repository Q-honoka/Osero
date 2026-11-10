#pragma once
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <iphlpapi.h>
#include <stdio.h>
#include <array>
#include "PacketHeader.h"
#include "Packet.h"

#pragma comment(lib, "Ws2_32.lib")

constexpr int MAX_CLIENTS = 2;		// 最大クライアント数
constexpr int MAX_RETRY_COUNT = 3;	// 最大再処理回数
constexpr int TIMEOUT_SECOND = 3;	// タイムアウトまでの時間

class NetWorkManager
{
public:
	NetWorkManager();	// コンストラクタ
	~NetWorkManager();	// デストラクタ
	bool SuccessInit() const;	// 初期化に成功したか返す
	bool StartConnect();	// 接続の受け付けを開始する
	bool AcceptClients();	// クライアントと接続する
	template<typename T>
	bool SendPacket(int num, const T& packet)	// クライアントにデータを送信する
	{
		const char* data =
			reinterpret_cast<const char*>(&packet);

		int totalSend = 0;
		int packetSize = sizeof(T);

		// すべてのデータが送信できるまで繰り返す
		while (totalSend < packetSize)
		{
			int iResult = send(clientSockets[num],
				data + totalSend,
				packetSize - totalSend,
				0);

			// エラーが起きた場合は false を返す
			if (iResult == SOCKET_ERROR)
			{
				return false;
			}

			totalSend += iResult;
			printf("送信したサイズ：%d 中の %d\n", packetSize, totalSend);
		}

		return true;
	}

	template<typename T>
	bool RecvPacket(T& packet, int clientNum)
	{
		// ヘッダを受信してデータの種類を確認する
		PacketHeader header;
		if (RecvHeader(header, clientNum) == false) return false;

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
				clientSockets[clientNum],
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
	bool MakeListenSocket();	// リッスン用ソケットを作成する
	bool BindListenSocket();	// リッスン用ソケットをバインドする
	bool RecvHeader(PacketHeader& h, int clientNum);	// ヘッダを受信する

	WSADATA wsaData;	// ソケット実装に必要な情報
	bool successInit;	// 初期化に成功したかどうか
	SOCKET listenSocket;	// リッスン用ソケット
	std::array<SOCKET, MAX_CLIENTS> clientSockets;	// クライアントとのデータのやり取り用ソケット配列
};