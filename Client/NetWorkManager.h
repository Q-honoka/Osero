#pragma once

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <iphlpapi.h>
#include <stdio.h>

#pragma comment(lib, "Ws2_32.lib")

class NetWorkManager
{
public:
	NetWorkManager();	// コンストラクタ
	bool SuccessInit() const;	// 初期化に成功したかどうかを返す
	bool ConnectServer();	// サーバーと接続する

private:
	WSADATA wsaData;	// ソケットの作成に必要な情報
	bool successInit;	// 初期化に成功したか
	SOCKET connectSocket;	// 接続用ソケット
};