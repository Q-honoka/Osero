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

#pragma comment(lib, "Ws2_32.lib")

constexpr int MAX_CLIENTS = 2;		// 最大クライアント数
constexpr int MAX_RETRY_COUNT = 3;	// 最大再処理回数
constexpr int TIMEOUT_SECOND = 3;	// タイムアウトまでの時間

class NetWorkManager
{
public:
	NetWorkManager();	// コンストラクタ
	bool SuccessInit() const;	// 初期化に成功したか返す
	bool StartConnect();	// 接続の受け付けを開始する
	bool AcceptClients();	// クライアントと接続する
	template<typename T>
	bool SendPacket(int num, const T& packet);	// クライアントにデータを送信する

private:
	bool MakeListenSocket();	// リッスン用ソケットを作成する
	bool BindListenSocket();	// リッスン用ソケットをバインドする

	WSADATA wsaData;	// ソケット実装に必要な情報
	bool successInit;	// 初期化に成功したかどうか
	SOCKET listenSocket;	// リッスン用ソケット
	std::array<SOCKET, MAX_CLIENTS> clientSockets;	// クライアントとのデータのやり取り用ソケット配列
};